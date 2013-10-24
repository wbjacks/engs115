// Includes
#include "./threads.h"

// Statics
static int find_user(void *ep, void *kp);

void connector(struct addrinfo *info) {
    int sub_socket;
    struct sockaddr_storage incoming_ip;
    socklen_t incoming_ip_len;
    pthread_t *rec_thread; 
    ReceiverInput_t *ri;

    // Create main socket and bind it
    if ((sockfd = socket(info->ai_family, info->ai_socktype, info->ai_protocol))
         == -1)
    {
        fprintf(stderr, "Error: Problem binding to socket.\n");
        fprintf(stderr, "Error is: %s.\n", strerror(errno));
        fprintf(stderr, "Exiting...\n");
        return;

    }
    if (bind(sockfd, info->ai_addr, info->ai_addrlen) == -1) {
        fprintf(stderr, "Error: Problem binding to socket.\n");
        fprintf(stderr, "Error is: %s.\n", strerror(errno));
        fprintf(stderr, "Exiting...\n");
        return;

    }
    printf("Socket bound to: %d.\n", sockfd);

    /* Connector loop! */
    for (;;) {
        // Listen for connects
        if (listen(sockfd, LISTEN_BACKLOG) == -1) {
            fprintf(stderr, "Error: Problem with listen.\n");
            fprintf(stderr, "Error is: %s.\n", strerror(errno));
            continue;

        }
        sleep(10);
        // Got something from listen, accept and launch a receiver
        incoming_ip_len = sizeof(incoming_ip);
        if ((sub_socket = accept(sockfd, (struct sockaddr *)&incoming_ip,
            &incoming_ip_len)) == -1)
        {
            fprintf(stderr, "Error: Problem accepting incoming connection.\n");
            fprintf(stderr, "Error is: %s.\n", strerror(errno));
            continue;

        }
        printf("Connection accepted!");
        //fflush(stdout);

        // Launch reciever, add to queue
        rec_thread = (pthread_t *)malloc(sizeof(pthread_t));
        ri = (ReceiverInput_t *)malloc(sizeof(ReceiverInput_t));
        ri->sub_socket = sub_socket;
        pthread_create(rec_thread, NULL, receiver, (void *)ri);
        pqput(connection_queue, rec_thread);

    }
}

/* Receiver manages a single socket and accepts it's input */
void *receiver(void *pt) {
    int bytes;
    char msg[MAX_MSG_LENGTH];
    char *str_element;
    MsgQueueElement_t *me_pt;
    ReceiverInput_t *input = (ReceiverInput_t *)pt;

    /* UNG UNG WHUT UP MAYNE LUUUP */
    for (;;) { // change this to check for a termination flag?
        memset(msg, 0, MAX_MSG_LENGTH); 
        if ((bytes = recv(input->sub_socket, msg, MAX_MSG_LENGTH-1, 0)) == -1)
        {
            fprintf(stderr, "Error: Problem receiving.\n");
            fprintf(stderr, "Error is: %s.\n", strerror(errno));

        }
        else if (bytes == 0) {
            // Connection closed, kill and exit
            printf("Connection closed on socket %d, closing...\n", input->sub_socket);
            close(input->sub_socket);
            break;

        }
        else {
            msg[bytes] = '\0';
            printf("Message received of length %d: %s.\n", bytes, msg);
            str_element = (char *)malloc(strlen(msg) + 1);
            strncpy(str_element, msg, strlen(msg)+1);

            // Create MsgElement_t
            me_pt = (MsgQueueElement_t *)malloc(sizeof(MsgQueueElement_t));
            me_pt->str = str_element;
            me_pt->sub_socket = input->sub_socket;
            pqput(msg_queue, (void *)me_pt);

        }
    }
    return NULL;

}

/* Parser takes input message and parses it into command structures */
void *parser(void *pt) {
    int src_id;
    MsgQueueElement_t *e_pt;
    char *tok, *save_pt;
    ChatCommand_t *cc;
    ChatUser_t *up;

    for (;;) {
        e_pt = (MsgQueueElement_t *)pqget(msg_queue);
        // Nothing in queue...
        if (!e_pt) {
            sleep(PARSER_SLEEP_TIME);

        }
        else {
            // Retrieved a message from the queue
            // Tokenize the message
            tok = strtok_r(e_pt->str, ":", &save_pt);
            
            // Construct struct
            cc = (ChatCommand_t *)malloc(sizeof(ChatCommand_t));
            cc->sub_socket = e_pt->sub_socket;
            
            // Switch on the token, fill in appropriate value
            if (!strncmp(tok, "msg", 3)) {
                cc->type = CF_MSG;
                tok = strtok_r(NULL, ":", &save_pt);
                src_id = atoi(tok);
                up = pqsearch(room->qp, find_user, &src_id);
                if (!up) break;
                tok = strtok_r(NULL, ":", &save_pt);
                cc->data = malloc(strlen(tok) + 1);
                cc->issuer = *up;
                strncpy(cc->data, tok, strlen(tok)+1);

            }
            else if (!strncmp(tok, "ping", 4)) {
                cc->type = CF_PING;

            }
            else if (!strncmp(tok, "join", 4)) {
                cc->type = CF_JOIN;
                // Construct a user object
                tok = strtok_r(NULL, ":", &save_pt);
                up = (ChatUser_t *)malloc(sizeof(ChatUser_t));
                strncpy(up->alias, tok, strlen(tok)+1);

                tok = strtok_r(NULL, ":", &save_pt);
                up->id = atoi(tok);
                up->sub_socket = cc->sub_socket;
                cc->data = up;

            }
            else if (!strncmp(tok, "leave", 5)) {
                cc->type = CF_LEAVE;
                // Get user from id in message
                tok = strtok_r(NULL, ":", &save_pt);
                src_id = atoi(tok);
                up = pqsearch(room->qp, find_user, &src_id);
                cc->issuer = *up;

                // Data becomes user_id

            }
            else if (!strncmp(tok, "who", 3)) {
                cc->type = CF_WHO;
                // Yayy no data

            }
            else {
                fprintf(stderr, "Error: Bad message received.\n");
                free(cc);
                cc = NULL;

            }
            // Add to queue
            if (cc) pqput(task_queue, (void *)cc);
                
        }
    }
    return NULL;

}

/* Dispatcher- Takes tasks from task queue */
void *dispatcher(void *pt) {
    char msg[MAX_MSG_LENGTH];
    ChatCommand_t *cc;
    char input[MAX_INPUT_LENGTH];

    // First dispatcher creates the chat room
    if (!room) {
        room = (ChatRoom_t *)malloc(sizeof(ChatRoom_t));
        printf("Please enter name of room: ");
        if (scanf("%s", input) == EOF) {
            fprintf(stderr, "Problem reading input.\n");
            fprintf(stderr, "Error is: %s.\n", strerror(errno));

        }
        strncpy(room->name, input, strlen(input)+1);
        room->id = sockfd;
        room->user_count = 0;
        room->qp = pqopen();
        pthread_mutex_init(&(room->lock), NULL);

    }

    // Grab task from pqueue
    for (;;) {
        cc = (ChatCommand_t *)pqget(task_queue);
        // Nothing in queue...
        if (!cc) {
            sleep(DISPATCHER_SLEEP_TIME);

        }
        else {
            memset(msg, 0, MAX_MSG_LENGTH);
            // switch on tast type, package msg
            switch (cc->type) {
                case CF_MSG:
                    printf("Chat message received!\n");
                    pqfold(room->qp, disp_msg, cc);
                    break;

                case CF_PING:
                    printf("Ping received!\n");
                    disp_ping(cc);
                    break;

                case CF_JOIN:
                    printf("Join received!\n");
                    disp_join(cc);
                    room->user_count++;
                    break;

                case CF_LEAVE:
                    printf("Leave received!\n");
                    disp_leave(cc);
                    room->user_count--;
                    break;

                case CF_WHO:
                    printf("Who received!\n");
                    disp_who(cc);
                    break;

                default:
                    fprintf(stderr, "Error: Bad struct in task queue. ");
                    fprintf(stderr, "This should not happen. PANIC.\n");

            }
        }
    }
    return NULL;

}

static int find_user(void *ep, void *kp) {
    return ((ChatUser_t *)ep)->id == *((int *)kp);

}

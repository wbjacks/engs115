// Arguments are IP and port
#include "./server.h"

// Statics
static void *connector(void *pt);
static void *receiver(void *pt);
static void *parser(void *pt);
static void *dispatcher(void *pt);

static void *disp_msg(void *el, void *pt);
static void disp_ping(ChatCommand_t *pt);
static void disp_join(ChatCommand_t *pt);
static void disp_leave(ChatCommand_t *pt);
static void disp_who(ChatCommand_t *pt);

static int find_user(void *ep, void *kp);
static void *curried_sprintf(void *ep, void *acc);

// Globals
void *msg_queue;
void *task_queue;
void *connection_queue;
int sockfd;
ChatRoom_t *room;


int main(int argc, char *argv[]) {
    int status;
    int i;
    void *st;
    struct addrinfo hints;
    struct addrinfo *info;
    pthread_t conn_thread, par_threads[NUM_PARSERS], disp_threads[NUM_DISPATCHERS];

    if (argc != 2) {
        fprintf(stderr, "Improper number of arguments. Exiting...\n");
        return EXIT_FAILURE;

    }
    /* Get info */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;        // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // UDP datagram sockets
    hints.ai_flags = AI_PASSIVE;      // fill in my IP for me

    if ((status = getaddrinfo(NULL, argv[1], &hints, &info)) != 0) {
        fprintf(stderr, "Error: Problem retrieving address information of input.\n");
        fprintf(stderr, "Error is: %s\n", gai_strerror(status));
        fprintf(stderr, "Exiting...\n");
        return EXIT_FAILURE;

    }

    // Create queues
    msg_queue = pqopen();
    task_queue = pqopen();
    connection_queue = pqopen();

    // Launch connector 
    pthread_create(&conn_thread, NULL, connector, (void *)info);

    // Launch parsers
    for (i = 0; i < NUM_PARSERS; i++)
        pthread_create(&par_threads[i], NULL, parser, NULL);

    // Launch dispatchers
    for (i = 0; i < NUM_DISPATCHERS; i++)
        pthread_create(&disp_threads[i], NULL, dispatcher, NULL);

    // Wait for all to join...
    pthread_join(conn_thread, &st);
    for (i = 0; i < NUM_PARSERS; i++)
        pthread_join(par_threads[i], &st);
    for (i = 0; i < NUM_DISPATCHERS; i++)
        pthread_join(disp_threads[i], &st);

    /* Clean and exit */
    freeaddrinfo(info);
    close(sockfd);
    return EXIT_SUCCESS;

}

/* Connector manages incoming connect()s from clients and launches recievers */
static void *connector(void *pt) {
    int sub_socket;
    struct sockaddr_storage incoming_ip;
    struct addrinfo *info = (struct addrinfo *)pt;
    socklen_t incoming_ip_len;
    ReceiverInput_t *ri;
    pthread_t *rec_thread;
    
    // Create main socket and bind it
    if ((sockfd = socket(info->ai_family, info->ai_socktype, info->ai_protocol))
         == -1)
    {
        fprintf(stderr, "Error: Problem binding to socket.\n");
        fprintf(stderr, "Error is: %s.\n", strerror(errno));
        fprintf(stderr, "Exiting...\n");
        return NULL;

    }
    if (bind(sockfd, info->ai_addr, info->ai_addrlen) == -1) {
        fprintf(stderr, "Error: Problem binding to socket.\n");
        fprintf(stderr, "Error is: %s.\n", strerror(errno));
        fprintf(stderr, "Exiting...\n");
        return NULL;

    }
    printf("Socket bound to: %d.\n", sockfd);

    for (;;) {
        // Listen for connects
        if (listen(sockfd, LISTEN_BACKLOG) == -1) {
            fprintf(stderr, "Error: Problem with listen.\n");
            fprintf(stderr, "Error is: %s.\n", strerror(errno));
            return NULL;

        }
        // Got something from listen, accept and launch a receiver
        incoming_ip_len = sizeof(incoming_ip);
        if ((sub_socket = accept(sockfd, (struct sockaddr *)&incoming_ip,
            &incoming_ip_len)) == -1)
        {
            fprintf(stderr, "Error: Problem accepting incoming connection.\n");
            fprintf(stderr, "Error is: %s.\n", strerror(errno));
            return NULL;

        }
        //sleep(10);
        //printf("Connection accepted!");
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
static void *receiver(void *pt) {
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
static void *parser(void *pt) {
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
static void *dispatcher(void *pt) {
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

static void *disp_msg(void *el, void *pt) {
    // Used for qapply 
    ChatCommand_t *cc = (ChatCommand_t *)pt;
    ChatUser_t *up = (ChatUser_t *)el;
    char *out_msg;

    if (cc->issuer.id != up->id) {
        // Add formatting
        out_msg = malloc(strlen((char *)cc->data) + strlen(cc->issuer.alias) + 3);
        sprintf(out_msg, "%s: %s", cc->issuer.alias, (char *)cc->data);
        
        if (send(up->sub_socket, out_msg, strlen(out_msg)+1, 0) == -1) {
            fprintf(stderr, "Error: Problem sending message.\n");
            fprintf(stderr, "Error is: %s.\n", strerror(errno));

        }
    }
    return pt;

}

static void disp_ping(ChatCommand_t *pt) {
    if (send(pt->sub_socket, "pong", (size_t)5, 0) == -1) {
        fprintf(stderr, "Error: Problem sending message.\n");
        fprintf(stderr, "Error is: %s.\n", strerror(errno));

    }
}

static void disp_join(ChatCommand_t *pt) {
    // Check that user id doesn't already exist in room
    if (!pqsearch(room->qp, find_user, &(((ChatUser_t *)pt->data)->id))) {
        // Add user to list
        pqput(room->qp, pt->data);

        // Return JOIN_OK message
        if (send(pt->sub_socket, "JOIN_OK", (size_t)8, 0) == -1) {
            fprintf(stderr, "Error: Problem sending message.\n");
            fprintf(stderr, "Error is: %s.\n", strerror(errno));

        }
    }
}

static void disp_leave(ChatCommand_t *pt) {
    ChatUser_t *up;

    // Remove user
    up = (ChatUser_t *)pqremove(room->qp, find_user, &(pt->issuer.id));
    if (up) {
        // Return REMOVE_OK message
        if (send(pt->sub_socket, "REMOVE_OK", (size_t)10, 0) == -1) {
            fprintf(stderr, "Error: Problem sending message.\n");
            fprintf(stderr, "Error is: %s.\n", strerror(errno));

        }
    }
}

static void disp_who(ChatCommand_t *pt) {
    // Construct who string
    char *who_str;
    who_str = (char *)malloc(sizeof("WHO_OK:\n"));
    strcpy(who_str, "WHO_OK:\n");
    pqfold(room->qp, curried_sprintf, who_str);

    // Send
    if (send(pt->sub_socket, who_str, strlen(who_str)+1, 0) == -1) {
        fprintf(stderr, "Error: Problem sending message.\n");
        fprintf(stderr, "Error is: %s.\n", strerror(errno));

    }
}

// Used for search
static int find_user(void *ep, void *kp) {
    return ((ChatUser_t *)ep)->id == *((int *)kp);

}

// Used for disp_who()
static void *curried_sprintf(void *ep, void *acc) {
    char *str = (char *)acc;
    char new_line[MAX_INPUT_LENGTH];

    sprintf(new_line, "%s -> %d\n", ((ChatUser_t *)ep)->alias, ((ChatUser_t *)ep)->id);
    str = (char *)realloc(str, strlen(str) + strlen(new_line) + 1);
    return strcat(str, new_line);

}

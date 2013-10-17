// Arguments are IP and port
#include "client.h"

// Static function prototypes
static char *get_message(void);
static void argument_help(void);
static void timeout_alarm(int signo);

static void *listener(void *pt);

static void send_cmd(struct addrinfo *info, int socket, int type, ChatUser_t *up);

int main(int argc, char *argv[]) {
    int sockfd;
    int status;
    char *input;
    char msg[MAX_INPUT_LENGTH];
    struct addrinfo hints, *info;
    pthread_t listener_thread;
    ChatUser_t *user;

    /* Get port input */
    // Check argument number
    if (argc != 3) {
        fprintf(stderr, "Error: Bad input arguments.\n");
        argument_help();
        fprintf(stderr, "Exiting...\n");
        return EXIT_FAILURE;

    }

    // Prepare for getadddinfo
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // UDP

    // Use ip, port to get address info
    if ((status = getaddrinfo(argv[1], argv[2], &hints, &info)) != 0) {
        fprintf(stderr, "Error: Problem retrieving address information of input.\n");
        fprintf(stderr, "Error is: %s\n", gai_strerror(status));
        fprintf(stderr, "Exiting...\n");
        return EXIT_FAILURE;

    }

    /* Open connection */
    printf("Opening connection on %s:%s...\n", argv[1], argv[2]);

    // Open socket
    sockfd = socket(info->ai_family, info->ai_socktype, info->ai_protocol);

    // Connect and alarm stuff
    signal(SIGALRM, timeout_alarm); // Theoretically, this works, but I have no idea how to test
    alarm(TIMEOUT_TIME);
    if (connect(sockfd, info->ai_addr, info->ai_addrlen) == -1) {
        fprintf(stderr, "Error: Problem connecting.\n");
        fprintf(stderr, "Error is: %s.\n", strerror(errno));
        return EXIT_FAILURE;

    }
    alarm(0);
    /* Connection has succeeded, begin running things... */
    printf("Connection success, socket is: %d.\n", sockfd);

    // Launch listener thread for incoming messages
    pthread_create(&listener_thread, NULL, listener, (void *)&sockfd);

    /* MAIN LOOP UP IN HUR */
    user = create_user();
    for(;;) {
        // Grab input
        printf("[%s]$ ", user->alias);
        /*
        if ((status = scanf("%s", input)) == EOF) {
            fprintf(stderr, "Error: Problem reading input.\n");
            fprintf(stderr, "Error is: %s.\n", strerror(errno));

        }
        */
        input = get_message();

        // Check for commands
        if (input[0] == '/') {
            switch (input[1]) { // Since all of the commands begin with // different letters
                case 'p': send_cmd(info, sockfd, CF_PING, user); break;
                case 'j': send_cmd(info, sockfd, CF_JOIN, user); break;
                case 'l': send_cmd(info, sockfd, CF_LEAVE, user); break;
                case 'w': send_cmd(info, sockfd, CF_WHO, user); break;
                default:
                    fprintf(stderr, "Command not recognized.\n");

            }
        }
        else if (!input[0]) {
            // I like pressing enter a lot and thus will treat it as dummy input
            continue;

        }
        else {
            // Chat message!
            sprintf(msg, "msg:%d:%s", user->id, input);
            if (sendto(sockfd, msg, strlen(msg)+1, 0,
                info->ai_addr, info->ai_addrlen) == -1)
            {
                fprintf(stderr, "Error: Problem sending message.\n");
                fprintf(stderr, "Error is: %s.\n", strerror(errno));

            }
        }
        // Cleanup
        //free(input);

    }

    /* Cleanup and return */
    close(sockfd);
    freeaddrinfo(info);
    return EXIT_SUCCESS;

}

static void timeout_alarm(int signo) {
    return;

}

static void send_cmd(struct addrinfo *info, int socket, int type, ChatUser_t *up) {
    int status;
    char out_msg[MAX_INPUT_LENGTH];
    
    switch (type) {
        case CF_PING:
            status = send(socket, "ping", 5, 0);
            break;
        case CF_JOIN:
            sprintf(out_msg, "join:%s:%d", up->alias, up->id);
            status = send(socket, out_msg, strlen(out_msg)+1, 0); 
            break;
        case CF_LEAVE:
            sprintf(out_msg, "leave:%d", up->id);
            status = send(socket, out_msg, strlen(out_msg)+1, 0); 
            break;
        case CF_WHO:
            status = send(socket, "who", 4, 0);
            break;
        default:
            fprintf(stderr, "Command not supported.\n");
            return;

    }

    if (status == -1) {
        fprintf(stderr, "Error: Problem sending message.\n");
        fprintf(stderr, "Error is: %s.\n", strerror(errno));
    }
    return;

}

static void *listener(void *pt) {
    char in_msg[MAX_INPUT_LENGTH];
    int bytes;

    for (;;) {
        memset(in_msg, 0, 5);
        if ((bytes = recv(*((int *)pt), in_msg, MAX_INPUT_LENGTH, 0)) == -1) {
            fprintf(stderr, "Error: Problem receiving.\n");
            fprintf(stderr, "Error is: %s.\n", strerror(errno));

        }
        else if (bytes == 0) {
            // Connection closed, kill and exit
            printf("Connection closed on socket %d, closing...\n", *((int *)pt));
            close(*((int *)pt));
            break;

        }
        else {
            printf("\n%s\n", in_msg);

        }
    }
    return NULL;

}

static char *get_message(void) {
    int current_length = 0;
    int max_length = 180;
    int realloc_size = max_length;
    int c;
    char *pt;

    pt = (char *)malloc(max_length * sizeof(char));

    while ((c = getchar()) != '\n') {
        pt[current_length++] = (char)c;
        if (current_length == (max_length - 2)) {
            pt = realloc(pt, max_length + realloc_size);
            max_length += realloc_size;

        }
    }
    // Consider eating newlines?
    pt[current_length++] = '\0';
    return pt;

}

static void argument_help(void) {
    printf("Arguments are:\n");
    printf("client ip_address port\n");
    printf("\tip_address: period delimited string of integers ");
    printf("representing the server location.\n");
    printf("\tport: integer representing the port endpoint on the server.\n\n");
    return;

}

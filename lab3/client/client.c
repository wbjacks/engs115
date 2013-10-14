// Arguments are IP and port

// Includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>

#include "../util/user.h"

// Defines
#define MAX_INPUT_LENGTH 100

// Static function prototypes
static char *get_message(void);
static void argument_help(void);

static void send_ping(struct addrinfo *info, int socket);
static void send_join(struct addrinfo *info, int socket);
static void send_who(struct addrinfo *info, int socket);
static void send_leave(struct addrinfo *info, int socket);

int main(int argc, char *argv[]) {
    int sockfd;
    int status;
    //char input[MAX_INPUT_LENGTH];
    char *input;
    char msg[MAX_INPUT_LENGTH];
    struct addrinfo hints, *info;
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
    hints.ai_socktype = SOCK_DGRAM; // UDP

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

    /* Connection has succeeded, begin running things... */
    printf("Connection success, socket is: %d.\n", sockfd);

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
                case 'p': send_ping(info, sockfd); break;
                case 'j': send_join(info, sockfd); break;
                case 'l': send_leave(info, sockfd); break;
                case 'w': send_who(info, sockfd); break;
                default:
                    fprintf(stderr, "Command not recognized.\n");

            }
        }
        else {
            // Chat message!
            sprintf(msg, "msg:%s", input);
            if (sendto(sockfd, msg, strlen(msg), 0,
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

static void send_ping(struct addrinfo *info, int socket) {
    struct sockaddr_storage incoming_ip;
    socklen_t incoming_ip_len;
    char msg[5];
    
    if (sendto(socket, "ping", (size_t)5, 0,
        info->ai_addr, info->ai_addrlen) == -1)
    {
        fprintf(stderr, "Error: Problem sending message.\n");
        fprintf(stderr, "Error is: %s.\n", strerror(errno));
    }
    else {
        // Wait for response from server
        memset(msg, 0, 5);
        if (recvfrom(socket, msg, 5, 0,
            (struct sockaddr *)&incoming_ip, &incoming_ip_len) == -1)
        {
            fprintf(stderr, "Error: Problem receiving.\n");
            fprintf(stderr, "Error is: %s.\n", strerror(errno));
            return;

        }
        // Message recieved, check for pong
        if (!strncmp(msg, "pong", 4)) {
            // Message matches, consider adding timing / ip display here
            printf("Server is up!\n");

        }
        else {
            // Maybe protect against message spamming here?
            printf("Bad message recieved: %s!\n", msg);

        }
    }
    return;

}

static void send_join(struct addrinfo *info, int socket) {}
static void send_who(struct addrinfo *info, int socket) {}
static void send_leave(struct addrinfo *info, int socket) {}

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

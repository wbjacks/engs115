// Arguments are IP and port

// Includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>

// Static function prototypes
static char *get_message(void);
static void argument_help(void);

int main(int argc, char *argv[]) {
    int sockfd;
    int status;
    struct addrinfo hints, *info;
    char *input;
    //struct sockaddr_in address;

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
    hints.ai_socktype = SOCK_STREAM; // TCP

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

    // Aaaand connect
    if (connect(sockfd, info->ai_addr, info->ai_addrlen) == -1) {
        fprintf(stderr, "Error: Connection failed.\n");
        fprintf(stderr, "Error is: %s.\n", strerror(errno));
        fprintf(stderr, "Exiting...\n");
        return EXIT_FAILURE;

    }

    /* Connection has succeeded, begin running things... */
    printf("Connection success!\n");

    /* MAIN LOOP UP IN HUR */
    for(;;) {
        printf("Send string: ");
        input = get_message();

        // Send to socket
        send(sockfd, input, sizeof input, 0);

    }

    /* Cleanup and return */
    close(sockfd);
    freeaddrinfo(info);
    return EXIT_SUCCESS;

}

static char *get_message(void) {
    int current_length = 0
    int max_length = 180;
    int realloc_size = max_size;
    char *pt;
    char c;

    pt = malloc(max_length * sizeof char);

    while ((c = getchar()) != '\n') {
        pt[current_length++] = c;
        if (current_length == (max_length - 2)) {
            pt = realloc(pt, max_length + realloc_size)

        }
    }
    // Consider eating newlines?
    pt[current_length] = '\0'
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

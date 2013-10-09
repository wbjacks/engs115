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

// Defines
#define LISTEN_BACKLOG 15
#define MAX_MSG_LENGTH 100

int main(int argc, char *argv[]) {
    int sockfd;
    int status;
    char msg[MAX_MSG_LENGTH];
    struct addrinfo hints;
    struct addrinfo *info;
    struct sockaddr_storage incoming_ip;
    socklen_t incoming_ip_len;

    if (argc != 2) {
        fprintf(stderr, "Improper number of arguments. Exiting...\n");
        return EXIT_FAILURE;

    }
    /* Get info */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;        // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_DGRAM; // UDP datagram sockets
    hints.ai_flags = AI_PASSIVE;      // fill in my IP for me

    if ((status = getaddrinfo(NULL, argv[1], &hints, &info)) != 0) {
        fprintf(stderr, "Error: Problem retrieving address information of input.\n");
        fprintf(stderr, "Error is: %s\n", gai_strerror(status));
        fprintf(stderr, "Exiting...\n");
        return EXIT_FAILURE;

    }
    /* Create socket */
    if ((sockfd = socket(info->ai_family, info->ai_socktype, info->ai_protocol))
         == -1)
    {
        fprintf(stderr, "Error: Problem binding to socket.\n");
        fprintf(stderr, "Error is: %s.\n", strerror(errno));
        fprintf(stderr, "Exiting...\n");
        return EXIT_FAILURE;

    }
    if (bind(sockfd, info->ai_addr, info->ai_addrlen) == -1) {
        fprintf(stderr, "Error: Problem binding to socket.\n");
        fprintf(stderr, "Error is: %s.\n", strerror(errno));
        fprintf(stderr, "Exiting...\n");
        return EXIT_FAILURE;

    }
    freeaddrinfo(info);
    printf("Socket bound to: %d.\n", sockfd);

    /* UNG UNG WHUT UP MAYNE LUUUP */
    for (;;) {
        memset(msg, 0, MAX_MSG_LENGTH); 
        incoming_ip_len = sizeof(incoming_ip);
        if ((status = recvfrom(sockfd, msg, MAX_MSG_LENGTH-1, 0,
            (struct sockaddr *)&incoming_ip, &incoming_ip_len)) == -1)
        {
            fprintf(stderr, "Error: Problem receiving.\n");
            fprintf(stderr, "Error is: %s.\n", strerror(errno));

        }
        else {
            msg[status] = '\0';
            printf("Message received of length %d: %s.\n", status, msg);

        }
    }
    /* Clean and exit */
    close(sockfd);
    return EXIT_SUCCESS;

}

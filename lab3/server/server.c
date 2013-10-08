// Arguments are IP and port

// Includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>

// Defines
#define LISTEN_BACKLOG 15
#define MAX_MSG_LENGTH 1000

int main(int argc, char *argv[]) {
    int sockfd, connected_sockfd;
    int status;
    char msg[MAX_MSG_LENGTH]
    struct addrinfo hints;
    struct addrinfo *info;
    struct sockaddr_storage incoming_addr;
    socklen_t incoming_addr_size;

    /* Get info */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
    hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

    if ((status = getaddrinfo(NULL, argv[1], &hints, &info)) != 0) {
        fprintf(stderr, "Error: Problem retrieving address information of input.\n");
        fprintf(stderr, "Error is: %s\n", gai_strerror(status));
        fprintf(stderr, "Exiting...\n");
        return EXIT_FAILURE;

    }

    /* Create socket */
    sockfd = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
    bind(sockfd, info->ai_addr, info->ai_addrlen);

    /* Listen for incoming packets */
    if (listen(sockfd, LISTEN_BACKLOG) == -1) {
        fprintf(stderr, "Error: Listen failed.\n");
        fprintf(stderr, "Error is: %s.\n", strerror(errno));
        fprintf(stderr, "Exiting...\n");
        return EXIT_FAILURE;
        
    }
    incoming_addr_size = sizeof(socklen_t);
    connected_sockfd = accept(sockfd, (struct sockaddr *)&incoming_addr, &incoming_addr_size);
    
    // Connected?
    printf("Received connection...");

    /* UNG UNG WHUT UP MAYNE LUUUP */
    for (;;) {
        recv(sockfd, ;

    }

    /* Clean and exit */
    close(sockfd);
    close(connected_sockfd);
    freeaddrinfo(info);
    return EXIT_SUCCESS;

}

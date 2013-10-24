// Arguments are IP and port
#include "./server.h"

int main(int argc, char *argv[]) {
    int status;
    int i;
    void *st;
    struct addrinfo hints;
    struct addrinfo *info;
    pthread_t par_threads[NUM_PARSERS], disp_threads[NUM_DISPATCHERS];

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

    // Launch parsers
    for (i = 0; i < NUM_PARSERS; i++)
        pthread_create(&par_threads[i], NULL, parser, NULL);

    // Launch dispatchers
    for (i = 0; i < NUM_DISPATCHERS; i++)
        pthread_create(&disp_threads[i], NULL, dispatcher, NULL);

    // Connection loop...
    connector(info);

    // Wait for all to join...
    for (i = 0; i < NUM_PARSERS; i++)
        pthread_join(par_threads[i], &st);
    for (i = 0; i < NUM_DISPATCHERS; i++)
        pthread_join(disp_threads[i], &st);

    /* Clean and exit */
    freeaddrinfo(info);
    close(sockfd);
    return EXIT_SUCCESS;

}

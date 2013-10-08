// Arguments are IP and port

// Includes
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

// Static function prototypes
static void argument_help(void);

int main(int argc, char *argv[]) {
    int sockfd;
    struct addrinfo hints;
    struct addrinfo *info;
    struct socket_addr address;

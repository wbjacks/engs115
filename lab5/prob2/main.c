#include <stdio.h>
#include "wkman.h"
#include "tkn.h"
#include "revtkn.h"

int main(int argc, char *argv[]) {
    // Arguments
    switch (atoi(argv[1])) {
        case 1: runWkMan(argc, argv); break;

        case 2: runTkn(argc, argv); break;

        case 3: runRevTkn(argc, argv); break;

        default:
            fprintf(stderr, "Error: bad input. Exiting...\n");
            break;

    }
    return EXIT_SUCCESS;

}



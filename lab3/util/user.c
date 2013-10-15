#include "./user.h"

ChatUser_t *create_user(void) {
    int status, c;
    char input[MAX_ALIAS_SIZE];
    ChatUser_t *u;

    printf("Please input user alias (max 99 chars, default ""): ");
    if ((status = scanf("%s", input)) == EOF) {
        fprintf(stderr, "Error: Problem reading input.\n");
        fprintf(stderr, "Error is: %s.\n", strerror(errno));
        return NULL;

    }
    // Eat extra newline
    while ((c = fgetc(stdin)) != EOF && c != '\n') {}
    u = (ChatUser_t *)malloc(sizeof(ChatUser_t));
    strncpy(u->alias, input, 91);
    u->id = (int)(MIN_USER_ID + (rand() / (double)RAND_MAX) * MAX_USER_ID);
    return u;

}

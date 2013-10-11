#include "./user.h"

ChatUser_t *create_user(void) {
    char input[MAX_ALIAS_SIZE];
    ChatUser_t *u;

    printf("Please input user alias (max 99 chars, default ""): ");
    scanf("%s", input);
    u = (ChatUser_t *)malloc(sizeof(ChatUser_t));
    strncpy(u->alias, input, 91);
    return u;

}

#include "commands.h"

// Statics
static int find_user(void *ep, void *kp);
static void *curried_sprintf(void *ep, void *acc);

void *disp_msg(void *el, void *pt) {
    // Used for qapply 
    ChatCommand_t *cc = (ChatCommand_t *)pt;
    ChatUser_t *up = (ChatUser_t *)el;
    char *out_msg;

    if (cc->issuer.id != up->id) {
        // Add formatting
        out_msg = malloc(strlen((char *)cc->data) + strlen(cc->issuer.alias) + 3);
        sprintf(out_msg, "%s: %s", cc->issuer.alias, (char *)cc->data);
        
        if (send(up->sub_socket, out_msg, strlen(out_msg)+1, 0) == -1) {
            fprintf(stderr, "Error: Problem sending message.\n");
            fprintf(stderr, "Error is: %s.\n", strerror(errno));

        }
    }
    return pt;

}

void disp_ping(ChatCommand_t *pt) {
    if (send(pt->sub_socket, "pong", (size_t)5, 0) == -1) {
        fprintf(stderr, "Error: Problem sending message.\n");
        fprintf(stderr, "Error is: %s.\n", strerror(errno));

    }
}

void disp_join(ChatCommand_t *pt) {
    // Check that user id doesn't already exist in room
    if (!pqsearch(room->qp, find_user, &(((ChatUser_t *)pt->data)->id))) {
        // Add user to list
        pqput(room->qp, pt->data);

        // Return JOIN_OK message
        if (send(pt->sub_socket, "JOIN_OK", (size_t)8, 0) == -1) {
            fprintf(stderr, "Error: Problem sending message.\n");
            fprintf(stderr, "Error is: %s.\n", strerror(errno));

        }
    }
}

void disp_leave(ChatCommand_t *pt) {
    ChatUser_t *up;

    // Remove user
    up = (ChatUser_t *)pqremove(room->qp, find_user, &(pt->issuer.id));
    if (up) {
        // Return REMOVE_OK message
        if (send(pt->sub_socket, "REMOVE_OK", (size_t)10, 0) == -1) {
            fprintf(stderr, "Error: Problem sending message.\n");
            fprintf(stderr, "Error is: %s.\n", strerror(errno));

        }
    }
}

void disp_who(ChatCommand_t *pt) {
    // Construct who string
    char *who_str;
    who_str = (char *)malloc(sizeof("WHO_OK:\n"));
    strcpy(who_str, "WHO_OK:\n");
    pqfold(room->qp, curried_sprintf, who_str);

    // Send
    if (send(pt->sub_socket, who_str, strlen(who_str)+1, 0) == -1) {
        fprintf(stderr, "Error: Problem sending message.\n");
        fprintf(stderr, "Error is: %s.\n", strerror(errno));

    }
}

// Used for search
static int find_user(void *ep, void *kp) {
    return ((ChatUser_t *)ep)->id == *((int *)kp);

}

// Used for disp_who()
static void *curried_sprintf(void *ep, void *acc) {
    char *str = (char *)acc;
    char new_line[MAX_INPUT_LENGTH];

    sprintf(new_line, "%s -> %d\n", ((ChatUser_t *)ep)->alias, ((ChatUser_t *)ep)->id);
    str = (char *)realloc(str, strlen(str) + strlen(new_line) + 1);
    return strcat(str, new_line);

}

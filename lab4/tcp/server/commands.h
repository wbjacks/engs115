// Includes
#include "../util/user.h"
#include "../util/pqueue.h"

// Defines
#define MAX_INPUT_LENGTH 100

// Structures
struct __chat_command {
    int type;
    void *data;
    ChatUser_t issuer;
    int sub_socket;

};

typedef struct __chat_command ChatCommand_t;
// Prototypes
void *disp_msg(void *el, void *pt);
void disp_ping(ChatCommand_t *pt);
void disp_join(ChatCommand_t *pt);
void disp_leave(ChatCommand_t *pt);
void disp_who(ChatCommand_t *pt);

// Globals
ChatRoom_t *room;

#define INPUT_BUFFER_LENGTH 99

struct __shopping_list {
    void *qp;
    int entry_count;

};

typedef struct __shopping_list Shopping_List_t;

struct __list_entry {
    char item_name[INPUT_BUFFER_LENGTH];
    int count;

};

typedef struct __list_entry List_Entry_t;


void printList(Shopping_List_t *sp);
void clearList(Shopping_List_t *sp);

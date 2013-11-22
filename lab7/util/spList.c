// Almost certainly should have a generic linked list object
struct __list_node {
    void *prob; // Problem as a packed buffer
    size_t *size;
    int is_subprob;
    struct __list_node next;
    struct __list_node prev;

};
typedef struct __list_node ListNode_t;

struct __list_head {
    ListNode_t *first;
    ListNode_t *last;
    int num_nodes;

};
typedef struct __list_head ListHead_t;

void *spLOpen() {
    void *ret;

    ret = malloc(sizeof(ListHead_t));
    ret->first = NULL;
    ret->last = NULL;
    ret->num_nodes = 0;

    return ret;

}

void spLClose(void *list) {
    ListNode_t *pt;
    ListHead_t *head = (ListHead_t *)list;

    for (pt = head->first; pt; pt = pt->next)
        free(pt);
    free(head);

}

int spLHasSubprob(void *list) {
    ListNode_t *pt;
    ListHead_t *head = (ListHead_t *)list;

    for (pt = head->first; pt; pt = pt->next)
        if (pt->is_subprob) return TRUE;

    return FALSE;

}

void *spLGetSubprob(void *list) {
    void *ret;
    ListNode_t *pt, *sp;
    ListHead_t *head = (ListHead_t *)list;

    for (pt = head->first; pt; pt = pt->next) {
        if (pt->is_subprob) {
            sp = pt;
            break;

        }
    }
    if (!sp) return NULL;
    // Remove sp
    if (sp->prev) sp->prev = sp->next;
    if (head->first == sp) head->first = sp->next;
    if (head->last == sp) head->last = sp->prev;

    ret = sp->prob;
    free(sp);
    return ret;

}

void *spLGetOrigProb(void *list) {
    void *ret;
    ListNode_t *pt, *sp;
    ListHead_t *head = (ListHead_t *)list;

    for (pt = head->first; pt; pt = pt->next) {
        if (!pt->is_subprob) {
            sp = pt;
            break;

        }
    }
    if (!sp) return NULL;
    // Remove sp
    if (sp->prev) sp->prev = sp->next;
    if (head->first == sp) head->first = sp->next;
    if (head->last == sp) head->last = sp->prev;

    ret = sp->prob;
    free(sp);
    return ret;

}


int spLAddProb(void *list, void *prob, size_t size, int is_subprob) {
    ListNode_t *pt;
    ListHead_t *head = (ListHead_t *)list;

    pt = (ListNode_t *)malloc(sizeof(ListNode_t));
    pt->size = size;
    pt->is_subprob = is_subprob;
    pt->prob = prob;

    // Insert
    pt->prev = head->last;
    if (head->last) head->last->next = pt;
    head->last = pt;
    pt->next = NULL;
    if (!head->first) head->first = pt;

}

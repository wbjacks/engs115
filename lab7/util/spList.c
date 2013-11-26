#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "./macros.h"

// Almost certainly should have a generic linked list object
struct __list_node {
    void *prob; // Problem as a packed buffer
    size_t size;
    int is_subprob;
    struct __list_node *next;
    struct __list_node *prev;

};
typedef struct __list_node ListNode_t;

struct __list_head {
    ListNode_t *first;
    ListNode_t *last;
    int num_nodes;

};
typedef struct __list_head ListHead_t;

void *spLOpen() {
    ListHead_t *ret;

    ret = (ListHead_t *)malloc(sizeof(ListHead_t));
    ret->first = NULL;
    ret->last = NULL;
    ret->num_nodes = 0;

    return (void *)ret;

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

void *spLGetProb(void *list, size_t *size) {
    void *ret = NULL;
    ListNode_t *pt; 
    ListHead_t *head = (ListHead_t *)list;

    pt = head->first;
    *size = 0;
    if (pt) {
        head->first = pt->next;
        if (head->first) head->first->prev = NULL;
        else head->last = NULL;
        *size = pt->size;
        ret = pt->prob;
        //free(pt);

    }
    return ret;

}

void *spLGetSubprob(void *list, size_t *size) {
    void *ret;
    ListNode_t *pt, *sp;
    ListHead_t *head = (ListHead_t *)list;

    sp = NULL;
    for (pt = head->first; pt; pt = pt->next) {
        if (pt->is_subprob) {
            sp = pt;
            break;

        }
    }
    if (!sp) return NULL;
    // Remove sp
    if (sp->prev) sp->prev->next = sp->next;
    if (sp->next) sp->next->prev = sp->prev;
    if (head->first == sp) head->first = sp->next;
    if (head->last == sp) head->last = sp->prev;

    ret = sp->prob;
    *size = sp->size;
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


void spLAddProb(void *list, void *prob, size_t size, int is_subprob) {
    ListNode_t *pt, *temp;
    ListHead_t *head = (ListHead_t *)list;

    pt = (ListNode_t *)malloc(sizeof(ListNode_t));
    pt->size = size;
    pt->is_subprob = is_subprob;
    pt->prob = prob;
    pt->next = NULL;
    pt->prev = NULL;

    // Insert
    // Check empty
    if (head->first == NULL && head->last == NULL) {
        head->first = pt;
        head->last = pt;
        return;

    }
    head->last->next = pt;
    temp = head->last;
    head->last = pt;
    pt->prev = temp;

}

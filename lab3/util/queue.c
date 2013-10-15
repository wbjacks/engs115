/******************************************************************************/
/*                                                                            */
/* File: queue.c                                                              */
/* Author: Will Jackson '14                                                   */
/* Date: Sep 26, 2013                                                         */
/*                                                                            */
/* Re-implementation of the queue code used in shop.c                         */
/*                                                                            */
/******************************************************************************/

/* Includes */
#include <stdlib.h>
#include <string.h>
#include "./queue.h"

/* Structures */
/* Queue will be implemented as a generic linked list, the inner workings of
   which can't be seen outside of this file. That is, everything can be simply
   controlled through the functions listed in queue.h */
struct __queue_element {
    void *element;
    struct __queue_element *prev;
    struct __queue_element *next;

};
typedef struct __queue_element Queue_Element_t;

struct __queue {
    Queue_Element_t *first;
    Queue_Element_t *last;

};
typedef struct __queue Queue_t;


public void *qopen(void) {
    void *qp;
    qp = malloc(sizeof(Queue_t));
    memset(qp, 0, sizeof(Queue_t));
    return qp;
}

public void qclose(void *qp) {
    qp = (Queue_t *)qp;
    free(qp);

}

public void qput(void *qp, void *elementp) {
    Queue_Element_t *pt;
    Queue_Element_t *entry;

    /* Create entry */
    entry = (Queue_Element_t *)malloc(sizeof(Queue_Element_t));
    memset(entry, 0, sizeof(Queue_Element_t));
    entry->element = elementp;

    /* Check empty */
    if (((Queue_t *)qp)->first == NULL && ((Queue_t *)qp)->last == NULL) {
        ((Queue_t *)qp)->first = entry;
        ((Queue_t *)qp)->last = entry;        

    }
    /* Common case */
    else {
        ((Queue_t *)qp)->last->next = entry;
        pt = ((Queue_t *)qp)->last;
        ((Queue_t *)qp)->last = entry;
        entry->prev = pt;

    }
}

public void* qget(void *qp) {
    Queue_Element_t *pt;
    void *ept;

    /* Check empty */
    if (((Queue_t *)qp)->first == NULL) return NULL; 

    ept = ((Queue_t *)qp)->first->element;
    pt = ((Queue_t *)qp)->first;
    ((Queue_t *)qp)->first = ((Queue_t *)qp)->first->next;
    if (((Queue_t *)qp)->first != NULL) {
        ((Queue_t *)qp)->first->prev = NULL;

    }
    else {
        /* Empty list  */
        ((Queue_t *)qp)->last = NULL;

    }
    free(pt);
    return ept;

}

public void qapply(void *qp, void (*fn)(void *elementp)) {
    Queue_Element_t *pt;
    pt = (Queue_Element_t *)((Queue_t *)qp)->first;
    while(pt != NULL) {
        (*fn)(pt->element);
        pt = pt->next;

    }
}

public void qfold(void *qp, void *(*fn)(void *elementp, void *accumulator), void *acc) {
    Queue_Element_t *pt;
    pt = (Queue_Element_t *)((Queue_t *)qp)->first;
    while(pt != NULL) {
        acc = (*fn)(pt->element, acc);
        pt = pt->next;

    }
}

public void *qsearch(void *qp, int (*searchfn)(void *elementp, void *keyp),
    void *skeyp)
{
    /* I am lazy and will be doing linear searching. I don't feel bad about 
       this, however, because more than likely, a shopping list will be
       a (relatively) small list of items, such that a n^2 algorithm will
       not be noticeably slower than a nlog(n) algorithm. So there.
    */
    Queue_Element_t *pt;
    pt = ((Queue_t *)qp)->first;
    while(pt != NULL) {
        if (searchfn(pt->element, skeyp)) break;
        pt = pt->next;

    }
    return pt;
}

public void *qremove(void *qp, int(*searchfn)(void *elementp, void *keyp),
    void *skeyp)
{
    Queue_Element_t *pt;

    /* Get element to remove */
    pt = (Queue_Element_t *)qsearch(qp, searchfn, skeyp);
    if (pt == NULL) return NULL;

    /* Remove element */
    if (pt == ((Queue_t*)qp)->last) {
        ((Queue_t *)qp)->last = pt->prev;

    }
    if (pt == ((Queue_t *)qp)->first) {
        ((Queue_t *)qp)->first = pt->next;

    }
    if (pt->next) pt->next->prev = pt->prev;
    if (pt->prev) pt->prev->next = pt->next;
    return pt->element;

}

public void qconcat(void *q1p, void *q2p) {
    /* Just stiches things together. The header doesn't specify that I need to
       deallocate q2p, so I won't. Might not be a terrible idea, though. */
    ((Queue_t *)q1p)->last->next = ((Queue_t *)q2p)->first;
    ((Queue_t *)q2p)->first->prev = ((Queue_t *)q1p)->last;
    ((Queue_t *)q1p)->last = ((Queue_t *)q2p)->last;

}

#include <stdlib.h>
#include <string.h>
#include "./queue.h"

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

/* Queue will be implemented as a generic linked list */

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
    if (((Queue_t *)qp)->first != NULL) ((Queue_t *)qp)->first->prev = NULL;
    free(pt);
    return ept;

}

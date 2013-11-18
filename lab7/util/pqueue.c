/******************************************************************************/
/* Structures */
/*                                                                            */
/* File: queue.c                                                              */
/* Author: Will Jackson '14                                                   */
/* Date: Sep 26, 2013                                                         */
/*                                                                            */
/* Re-implementation of the queue code used in shop.c                         */
/*                                                                            */
/******************************************************************************/

/* Includes */
#include "./pqueue.h"

/* Queue will be implemented as a generic linked list, the inner workings of
   which can't be seen outside of this file. That is, everything can be simply
   controlled through the functions listed in queue.h */

// Internal Structures
struct __pqueue {
    void *queue;
    pthread_mutex_t lock;

};
typedef struct __pqueue PQueue_t;

public void *pqopen(void) {
    PQueue_t *qp;
    qp = (PQueue_t *)malloc(sizeof(PQueue_t));
    pthread_mutex_init(&(qp->lock), NULL);
    qp->queue = qopen();
    return qp;
}

public void pqclose(void *qp) {
    pthread_mutex_destroy(&(((PQueue_t *)qp)->lock));
    qclose(((PQueue_t *)qp)->queue);
    free(qp);

}

public void pqput(void *qp, void *elementp, size_t size) {
    pthread_mutex_lock(&(((PQueue_t *)qp)->lock));
    qput(((PQueue_t *)qp)->queue, elementp, size);
    pthread_mutex_unlock(&(((PQueue_t *)qp)->lock));

}

public void* pqget(void *qp, size_t *size) {
    void *ept;

    pthread_mutex_lock(&(((PQueue_t *)qp)->lock));
    ept = qget(((PQueue_t *)qp)->queue, size);
    pthread_mutex_unlock(&(((PQueue_t *)qp)->lock));
    return ept;

}

public void pqapply(void *qp, void (*fn)(void *elementp)) {
    pthread_mutex_lock(&(((PQueue_t *)qp)->lock));
    qapply(((PQueue_t *)qp)->queue, fn);
    pthread_mutex_unlock(&(((PQueue_t *)qp)->lock));

}

public void pqfold(void *qp, void *(*fn)(void *elementp, void *accumulator), void *acc) {
    pthread_mutex_lock(&(((PQueue_t *)qp)->lock));
    qfold(((PQueue_t *)qp)->queue, fn, acc);
    pthread_mutex_unlock(&(((PQueue_t *)qp)->lock));

}

public void *pqsearch(void *qp, int (*searchfn)(void *elementp, void *keyp),
    void *skeyp)
{
    void *ept;

    pthread_mutex_lock(&(((PQueue_t *)qp)->lock));
    ept = qsearch(((PQueue_t *)qp)->queue, searchfn, skeyp);
    pthread_mutex_unlock(&(((PQueue_t *)qp)->lock));
    return ept;

}

public void *pqremove(void *qp, int(*searchfn)(void *elementp, void *keyp),
    void *skeyp)
{
    void *ept;

    pthread_mutex_lock(&(((PQueue_t *)qp)->lock));
    ept = qremove(((PQueue_t *)qp)->queue, searchfn, skeyp);
    pthread_mutex_unlock(&(((PQueue_t *)qp)->lock));
    return ept;

}

public void pqconcat(void *q1p, void *q2p) {
    pthread_mutex_lock(&(((PQueue_t *)q1p)->lock));
    pthread_mutex_lock(&(((PQueue_t *)q2p)->lock));
    pqconcat(((PQueue_t *)q1p)->queue, ((PQueue_t *)q2p)->queue);
    pthread_mutex_unlock(&(((PQueue_t *)q2p)->lock));
    pthread_mutex_unlock(&(((PQueue_t *)q1p)->lock));

}

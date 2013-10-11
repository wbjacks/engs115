// Includes
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "./queue.h"

// Defines
#define public
#define private static

// Structs
struct __pqueue {
    void *queue;
    pthread_mutex_t lock;// = PTHREAD_MUTEX_INITIALIZER;

};
typedef struct __pqueue PQueue_t;

// Prototypes
/* create an empty queue */
public void* pqopen(void);        

/* deallocate a queue, assuming every element has been removed and deallocated */
public void pqclose(void *qp);   

/* put element at end of queue */
public void pqput(void *qp, void *elementp); 

/* get first element from a queue */
public void* pqget(void *qp);

/* apply a void function (e.g. a printing fn) to every element of a queue */
public void pqapply(void *qp, void (*fn)(void* elementp));

/* search a queue using a supplied boolean function, returns an element */
public void* pqsearch(void *qp, 
		     int (*searchfn)(void* elementp,void* keyp),
		     void* skeyp);

/* search a queue using a supplied boolean function, removes an element */
public void* pqremove(void *qp,
		     int (*searchfn)(void* elementp,void* keyp),
		     void* skeyp);

/* concatenatenates q2 onto q1, q2 may not be subsequently used */
public void pqconcat(void *q1p, void *q2p);

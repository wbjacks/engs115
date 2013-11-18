// Includes
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "./queue.h"

// Defines
#define public
#define private static

// Prototypes
/* create an empty queue */
public void* pqopen(void);        

/* deallocate a queue, assuming every element has been removed and deallocated */
public void pqclose(void *qp);   

/* put element at end of queue with specified size */
public void pqput(void *qp, void *elementp, size_t size); 

/* get first element from a queue, size is filled in */
public void* pqget(void *qp, size_t *size);

/* apply a void function (e.g. a printing fn) to every element of a queue */
public void pqapply(void *qp, void (*fn)(void* elementp));

// Added by will: apply with a function that has an accumulator
public void pqfold(void *qp, void *(*fn)(void *elementp, void *accumulator), void *acc);

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

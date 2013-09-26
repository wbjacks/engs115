/* Unit tester for home-spun queue implimentation */
#include <stdlib.h>
#include <stdio.h>
#include "./queue.h"


void printfn(void *pt);
int searchfn(void *elementp, void *keyp);

int main(void) {
    /* Create two empty queues */
    void *q1;
    void *q2;
    void *pt;
    int a,b,c,d,e,f,g,h,i,j,k;
    int x;
    int ii;
    a = 1;
    b = 4;
    c = 65;
    d = 34;
    f = 6;
    e = 9;
    g = 11;
    h = 23;
    i = 999;
    j = 234;
    k = 94;

    q1 = qopen();
    q2 = qopen();

    /* Fill both */
    qput(q1, &a);
    qput(q1, &b);
    qput(q1, &c);
    qput(q1, &d);
    qput(q1, &e);
    qput(q1, &f);
    qput(q2, &g);
    qput(q2, &h);
    qput(q2, &i);
    qput(q2, &j);
    qput(q2, &k);

    /* Get an item out of q1 */
    pt = qget(q1);
    if (*((int *)pt) != 1) {
        printf("Fail on qget; returned %d.\n", *((int *)pt));
        return -1;

    }

    /* Catenate */
    qconcat(q1, q2);

    /* Remove until null is returned, count number of iterations */
    for (ii = 0; pt != NULL; ii++) {
        pt = qget(q1);

    }
    if (ii != 11) {
        printf("Fail on catenate; queue was of size %d.\n", ii);
        return -1;

    }

    /* Use remove to take out a specified value  */
    qput(q1, &a);
    qput(q1, &b);
    qput(q1, &c);
    qput(q1, &d);
    qput(q1, &e);
    qput(q1, &f);
    qput(q1, &g);
    qput(q1, &h);
    qput(q1, &i);
    qput(q1, &j);
    qput(q1, &k);
    x = 999;
    pt = qremove(q1, searchfn, &x);
    if (pt == NULL || *((int *)pt) != 999) {
        if (pt == NULL) {
            printf("Fail on qsearch; value not found.\n");

        }
        else {
            printf("Fail on qremove / qsearch; removed value was %d.\n", *((int *)pt));

        }
    }
    /* Use qapply to print out remaining list */
    qapply(q1, printfn);
    return 1;

}

void printfn(void *pt) {
    printf("%d\n", *((int *)pt));

}

int searchfn(void *elementp, void *keyp) {
    if (*((int *)elementp) == *((int *)keyp)) {
        return 1;

    }
    else {
        return 0;

    }
}

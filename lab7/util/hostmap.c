// Includes
#include <errno.h>

// Defines
#define MAX_IPV6_SIZE 46

#define GET_HOST(str) if (gethostname(str, MAX_IPV6_SIZE) != 0) { \
        fprintf(stderr, "Error in hostmap.c: Problem adding host.\n"); \
        fprintf(stderr, "Error is: %s.\n", strerror(errno)); \
        return -1;}

// Structs
struct __host_node {
    char address[MAX_IPV6_SIZE];
    int val;
    struct __host_node *next;
    struct __host_node *prev;

};
typedef struct __host_node HostNode_t;

struct __list_head {
    HostNode_t *first;
    HostNode_t *last;
    int num_nodes;

};
typedef struct __list_head ListHead_t;

void *createMap() {
    ListHead_t *m;
    m = (ListHead_t *)malloc(sizeof(ListHead_t));
    m->first = NULL;
    m->last = NULL;
    m->num_nodes = 0;

    return (void *)m;

}

void destroyMap(void *map) {
    HostNode_t *pt;
    ListHead_t *head = (ListHead_t *)map;

    for (pt = head->first; pt; pt = pt->next)
        free(pt);
    free(map);

}

int addCurrentHost(void *map) {
    ListHead_t *head = (ListHead_t *)map;
    HostNode_t *host;

    // Check if host exists
    if (hostExists(map)) return TRUE;

    // If it doesn't, create host
    host = (HostNode_t *)malloc(sizeof(HostNode_t));
    GET_HOST(host->address);

    // Insert new host
    if (head->last) head->last->next = host;
    host->prev = head->last;
    host->next = NULL;
    head->last = host;
    if (!head->first) head->first = host;
    head->num_nodes++;

    return FALSE;

}

int hostExists(void *map) {
    char curr_host[MAX_IPV6_SIZE];
    HostNode_t *pt;
    ListHead_t *head = (ListHead_t *)map;

    // Get current host
    GET_HOST(host->address);

    for (pt = head->first; pt; pt = pt->next) {
        if (!strcmp(curr_host, pt->address))
            return TRUE;

    }
    return FALSE;

}

void addValToHost(void *map, int val) {
    HostNode_t *pt;
    ListHead_t *head = (ListHead_t *)map;

    // Get current host
    GET_HOST(host->address);

    // Add value
    for (pt = head->first; pt; pt = pt->next) {
        if (!strcmp(curr_host, pt->address)) {
            pt->val += val;
            return;

        }
    }
    // Host doesn't exist, throw error
    fprintf(stderr, "Error in addValToHost(): current host doesn't exist in map.\n");

}

int *getAllValues(void *map, size_t *ret_size) {
   //int *vals = (int *)malloc();

}

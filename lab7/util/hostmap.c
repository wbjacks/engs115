// Includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "./hostmap.h"

// Defines
#define MAX_IPV6_SIZE 46

#define GET_HOST(str) do {if (gethostname(str, MAX_IPV6_SIZE) != 0) { \
        fprintf(stderr, "Error in hostmap.c: Problem adding host.\n"); \
        fprintf(stderr, "Error is: %s.\n", strerror(errno)); \
        return -1;} } while(0);

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
    GET_HOST(curr_host);

    for (pt = head->first; pt; pt = pt->next) {
        if (!strcmp(curr_host, pt->address))
            return TRUE;

    }
    return FALSE;

}

int addValToHost(void *map, int val) {
    char curr_host[MAX_IPV6_SIZE];
    HostNode_t *pt;
    ListHead_t *head = (ListHead_t *)map;

    // Get current host
    GET_HOST(curr_host);

    // Add value
    for (pt = head->first; pt; pt = pt->next) {
        if (!strcmp(curr_host, pt->address)) {
            pt->val += val;
            return 0;

        }
    }
    // Host doesn't exist, throw error
    fprintf(stderr, "Error in addValToHost(): current host doesn't exist in map.\n");
    return -1;

}

int *getAllValues(void *map, size_t *ret_size) {
    int *vals, *i_pt;
    HostNode_t *n_pt;
    ListHead_t *head = (ListHead_t *)map;

    // Build output array
    vals = (int *)malloc(head->num_nodes * sizeof(int));
    for (n_pt = head->first, i_pt = vals; n_pt; n_pt = n_pt->next)
        *i_pt++ = n_pt->val;

    *ret_size = head->num_nodes;
    return vals;

}

char **getAllHosts(void *map, size_t *ret_size) {
    char **ret, **s_pt;
    HostNode_t *n_pt;
    ListHead_t *head = (ListHead_t *)map;

    ret = malloc(head->num_nodes);
    for (n_pt = head->first, s_pt = ret; n_pt; n_pt = n_pt->next) {
        *s_pt = malloc(MAX_IPV6_SIZE);
        strncpy(*s_pt++, n_pt->address, MAX_IPV6_SIZE);

    }
    *ret_size = head->num_nodes;
    return ret;

}

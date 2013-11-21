// hostmap.h- Used to create a map of hosts to integers
#include "./macros.h"

// Creates an empty map
void *createMap();

// Safely cleans up an existing map
// void *map: pointer to map
void destroyMap(void *map);

// Adds the current host to the map. If the host already exists in the map,
// does nothing and returns TRUE. Else, adds the host with value = 0 and
// returns FALSE. Returns -1 on error.
// void *map: pointer to map
int addCurrentHost(void *map);

// Checks if the current host exists
// void *map: pointer to map
int hostExists(void *map);

// Adds the specified value to the current host. Throws an error if the host
// doesn't exist.
// void *map: pointer to map
// int val: value to add
void addValToHost(void *map, int val);

// Returns array of values in the map in set order.
// void *map: pointer to map
// size_t *ret_size: set with size of returned array
int *getAllValues(void *map, size_t *ret_size);

// Returns array of host strings in the map in set order.
// void *map: pointer to map
// size_t *ret_size: set with size of returned array
char **getAllHosts(void *map, size_t *ret_size);

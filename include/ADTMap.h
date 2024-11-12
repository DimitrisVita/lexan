#include "common.h"
#include "ADTHash.h"

// Implementations of ADTMap using ADTHash

typedef struct map* Map;

// Create a new map
Map createMap(int size);

// Add a new node to the map
void addMapNode(Map map, char *key, void *value);

// Delete a node from the map
void deleteMapNode(Map map, char *key);

// Get the value given a key
void *getMapValue(Map map, char *key);

// Get the size of the map
int getMapSize(Map map);

// Get first not null node


// Free the map
void freeMap(Map map);
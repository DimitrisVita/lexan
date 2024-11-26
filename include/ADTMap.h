#pragma once
#include "common.h"

#include "ADTHash.h"

// Implementations of ADTMap using ADTHash

typedef struct map* Map;

// Rename the hashnode to mapnode
typedef struct hash_node* MapNode;

struct map {
    Hashtable hashtable;
};

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
MapNode getFirstMapNode(Map map);

// Get next not null node
MapNode getNextMapNode(Map map, MapNode node);

// Free the map
void freeMap(Map map);

#include "ADTMap.h"

// Implementations of ADTMap using ADTHash

struct map {
    Hashtable hashtable;
};

// Create a new map
Map createMap(int size) {
    Map map = (Map)malloc(sizeof(struct map));
    if (map == NULL) {
        fprintf(stderr, "Could not allocate memory for map.\n");
        exit(1);
    }

    map->hashtable = createHashtable(size);
    return map;
}

// Add a new node to the map
void addMapNode(Map map, char *key, void *value) {
    addHashNode(map->hashtable, key, value);
}

// Delete a node from the map
void deleteMapNode(Map map, char *key) {
    deleteHashNode(map->hashtable, key);
}

// Get the value given a key
void *getMapValue(Map map, char *key) {
    return getHashNode(map->hashtable, key);
}

// Get the size of the map
int getMapSize(Map map) {
    return getHashtableSize(map->hashtable);
}

// Free the map
void freeMap(Map map) {
    freeHashtable(map->hashtable);
    free(map);
}

#include "ADTMap.h"

// Implementations of ADTMap using ADTHash

Map createMap(int size) {
    Map map = (Map) malloc(sizeof(struct map));
    if (map == NULL) {
        // Handle memory allocation failure
        return NULL;
    }
    map->hashtable = createHashtable(size);
    if (map->hashtable == NULL) {
        // Handle hashtable creation failure
        free(map);
        return NULL;
    }
    return map;
}

void addMapNode(Map map, char *key, void *value) {
    addHashNode(map->hashtable, key, value);
}

void deleteMapNode(Map map, char *key) {
    deleteHashNode(map->hashtable, key);
}

void *getMapValue(Map map, char *key) {
    return getHashNode(map->hashtable, key);
}

int getMapSize(Map map) {
    return getHashtableSize(map->hashtable);
}

MapNode getFirstMapNode(Map map) {
    return getFirstNode(map->hashtable);
}

MapNode getNextMapNode(Map map, MapNode node) {
    return getNextNode(map->hashtable, node);
}

void freeMap(Map map) {
    if (map != NULL) {
        freeHashtable(map->hashtable);
        free(map);
    }
}
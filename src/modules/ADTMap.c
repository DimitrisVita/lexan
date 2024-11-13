#include "ADTMap.h"

// Implementations of ADTMap using ADTHash

Map createMap(int size) {
    Map map = (Map) malloc(sizeof(struct map));
    map->hashtable = createHashtable(size);
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
    freeHashtable(map->hashtable);
    free(map);
}
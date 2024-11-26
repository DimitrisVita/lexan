#include "ADTHash.h"

// Create a new hashtable
Hashtable createHashtable(int size) {
    Hashtable hashtable = (Hashtable)malloc(sizeof(struct hash));
    if (hashtable == NULL) {
        fprintf(stderr, "Could not allocate memory for hashtable.\n");
        exit(1);
    }

    hashtable->size = size;
    hashtable->count = 0;

    hashtable->array = (HashNode *)malloc(hashtable->size * sizeof(HashNode));
    if (hashtable->array == NULL) {
        fprintf(stderr, "Could not allocate memory for hashtable array.\n");
        free(hashtable);
        exit(1);
    }

    for (int i = 0; i < hashtable->size; i++) {
        hashtable->array[i] = NULL; // Initialize all pointers to NULL
    }

    return hashtable;
}

// Hash function that uses sdmb hash
unsigned int hashFunction(char *key, int size) {
    unsigned int hash = 0;
    for (int i = 0; key[i] != '\0'; i++)
        hash = key[i] + (hash << 6) + (hash << 16) - hash;
    return hash % size;
}

// Add a new node to the hashtable
void addHashNode(Hashtable hashtable, char *key, void *value) {
    HashNode newNode = (HashNode)malloc(sizeof(struct hash_node));
    if (newNode == NULL) {
        fprintf(stderr, "Could not allocate memory for new node.\n");
        return;
    }

    newNode->key = strdup(key);
    if (newNode->key == NULL) {
        fprintf(stderr, "Could not allocate memory for key.\n");
        free(newNode);
        return;
    }

    newNode->value = value;
    unsigned int hashValue = hashFunction(key, hashtable->size);

    // Insert the new node at the beginning of the linked list
    newNode->next = hashtable->array[hashValue];
    hashtable->array[hashValue] = newNode;
    hashtable->count++;
}

// Remove a node from the hashtable
void deleteHashNode(Hashtable hashtable, char *key) {
    unsigned int hashValue = hashFunction(key, hashtable->size);
    HashNode node = hashtable->array[hashValue];
    HashNode prev = NULL;

    while (node != NULL) {
        if (strcmp(node->key, key) == 0) {
            if (prev == NULL)
                hashtable->array[hashValue] = node->next;
            else
                prev->next = node->next;
            free(node->key);
            free(node);
            hashtable->count--;
            return;
        }
        prev = node;
        node = node->next;
    }
}

// Get the value given a key
void *getHashNode(Hashtable hashtable, char *key) {
    unsigned int hashValue = hashFunction(key, hashtable->size);
    HashNode node = hashtable->array[hashValue];

    while (node != NULL) {
        if (strcmp(node->key, key) == 0)
            return node->value;
        node = node->next;
    }
    return NULL;
}

// Get the size of the hashtable
int getHashtableSize(Hashtable hashtable) {
    return hashtable->count;
}

// Get first non-null node
HashNode getFirstNode(Hashtable hashtable) {
    for (int i = 0; i < hashtable->size; i++) {
        if (hashtable->array[i] != NULL)
            return hashtable->array[i];
    }
    return NULL;
}

// Get next non-null node
HashNode getNextNode(Hashtable hashtable, HashNode node) {
    if (node->next != NULL)
        return node->next;
    for (int i = hashFunction(node->key, hashtable->size) + 1; i < hashtable->size; i++) {
        if (hashtable->array[i] != NULL)
            return hashtable->array[i];
    }
    return NULL;
}

// Free the hashtable
void freeHashtable(Hashtable hashtable) {
    for (int i = 0; i < hashtable->size; i++) {
        HashNode node = hashtable->array[i];
        while (node != NULL) {
            HashNode nextEntry = node->next;
            free(node->key);
            free(node);
            node = nextEntry;
        }
    }
    free(hashtable->array);
    free(hashtable);
}
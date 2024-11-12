#include "ADTHash.h"

// Create a new hashtable
Hashtable createHashtable(int size) {
    // Allocate memory for the hashtable
    Hashtable hashtable = (Hashtable)malloc(sizeof(struct hash));
    if (hashtable == NULL) {
        fprintf(stderr, "Could not allocate memory for hashtable.\n");
        exit(1);
    }

    // Initialize the hashtable
    hashtable->size = size;
    hashtable->count = 0;

    // Allocate memory for the array
    hashtable->array = (HashNode *)malloc(hashtable->size * sizeof(HashNode));
    if (hashtable->array == NULL) {
        fprintf(stderr, "Could not allocate memory for hashtable array.\n");
        exit(1);
    }

    // Initialize the array
    for (int i = 0; i < hashtable->size; i++) {
        hashtable->array[i] = NULL;    // set all pointers to NULL
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
    // Allocate memory for the new node
    HashNode newNode = (HashNode)malloc(sizeof(struct hash_node));
    if (newNode == NULL) {
        fprintf(stderr, "Could not allocate memory for new node.\n");
        return;
    }

    // Duplicate the key to ensure it is properly managed
    newNode->key = strdup(key);
    if (newNode->key == NULL) {
        fprintf(stderr, "Could not allocate memory for key.\n");
        free(newNode);
        return;
    }

    // Initialize the new node
    newNode->value = value;

    // Get the hash value of the key
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
    while (node != NULL) {  // iterate over the linked list
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
        prev = node;    // keep track of the previous node
        node = node->next;  // move to the next node
    }
}

// Get the value given a key
void *getHashNode(Hashtable hashtable, char *key) {
    unsigned int hashValue = hashFunction(key, hashtable->size);
    HashNode node = hashtable->array[hashValue];
    while (node != NULL) {  // iterate over the linked list
        if (strcmp(node->key, key) == 0)    // key found
            return node->value;
        node = node->next;
    }
    return NULL;    // key not found
}

// Get the size of the hashtable
int getHashtableSize(Hashtable hashtable) {
    return hashtable->count;
}

// Get first not null node
HashNode getFirstNode(Hashtable hashtable) {
    for (int i = 0; i < hashtable->size; i++) {
        if (hashtable->array[i] != NULL)
            return hashtable->array[i];
    }
    return NULL;
}

// Get next not null node
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
        HashNode node = hashtable->array[i];    // get the first node of the linked list
        while (node != NULL) {
            HashNode nextEntry = node->next;    // get the next node before freeing the current node
            free(node->key);
            free(node);
            node = nextEntry;   // move to the next node
        }
    }
    free(hashtable->array);
    free(hashtable);
}
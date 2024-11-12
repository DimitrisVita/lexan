#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct hash* Hashtable;
typedef struct hash_node* HashNode;

struct hash_node {
    char *key;
    void *value;
    HashNode next;  // pointer to the next node in the linked list
};

struct hash {
    int size;
    int count;
    HashNode *array;    // array of pointers to the first node of the linked lists
};

// Create a new hashtable
Hashtable createHashtable(int size);

// Hash function that uses sdmb hash
unsigned int hashFunction(char *key, int size);

// Add a new node to the hashtable
void addHashNode(Hashtable hashtable, char *key, void *value);

// Delete a node from the hashtable
void deleteHashNode(Hashtable hashtable, char *key);

// Get the value given a key
void *getHashNode(Hashtable hashtable, char *key);

// Get the size of the hashtable
int getHashtableSize(Hashtable hashtable);

// Get first not null node
HashNode getFirstNode(Hashtable hashtable);

// Get next not null node
HashNode getNextNode(Hashtable hashtable, HashNode node);

// Free the hashtable
void freeHashtable(Hashtable hashtable);
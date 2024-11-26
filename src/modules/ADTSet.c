#include "ADTSet.h"

// Implementations of ADTSet using ADTHash

struct set {
    Hashtable hashtable;  // Underlying hashtable to store set elements
};

// Create a new set
Set createSet(int size) {
    Set set = (Set)malloc(sizeof(struct set));
    if (set == NULL) {
        fprintf(stderr, "Could not allocate memory for set.\n");
        exit(1);
    }

    set->hashtable = createHashtable(size);
    return set;
}

// Add an element to the set
void addElement(Set set, char *element) {
    addHashNode(set->hashtable, element, element);  // Use element as both key and value
}

// Remove an element from the set
void removeElement(Set set, char *element) {
    deleteHashNode(set->hashtable, element);
}

// Check if an element is in the set
int containsElement(Set set, char *element) {
    return getHashNode(set->hashtable, element) != NULL;
}

// Free the set
void freeSet(Set set) {
    freeHashtable(set->hashtable);
    free(set);
}
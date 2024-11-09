#include "common.h"
#include "ADTHash.h"

// Implementations of ADTSet using ADTHash

typedef struct set* Set;

// Create a new set
Set createSet(int size);

// Add an element to the set
void addElement(Set set, char *element);

// Remove an element from the set
void removeElement(Set set, char *element);

// Check if an element is in the set
int containsElement(Set set, char *element);

// Free the set
void freeSet(Set set);
#include "ADTVector.h"

// Create a new vector
Vector createVector(int size) {
    // Allocate memory for the vector
    Vector vector = (Vector)malloc(sizeof(struct vector));
    if (vector == NULL) {
        fprintf(stderr, "Could not allocate memory for vector.\n");
        exit(1);
    }

    // Initialize the vector
    vector->size = size;
    vector->array = (VectorNode)malloc(size * sizeof(struct vector_node));
    if (vector->array == NULL) {
        fprintf(stderr, "Could not allocate memory for vector array.\n");
        free(vector);
        exit(1);
    }

    // Initialize the array
    for (int i = 0; i < size; i++) {
        vector->array[i].data = NULL;
    }

    return vector;
}

// Add a new node to the vector 
void addVectorNode(Vector vector, void *data) {
    // Find the first empty node in the vector
    for (int i = 0; i < vector->size; i++) {
        if (vector->array[i].data == NULL) {
            vector->array[i].data = data;
            return;
        }
    }

    // If the vector is full, double its size
    int oldSize = vector->size;
    vector->size *= 2;
    vector->array = (VectorNode)realloc(vector->array, vector->size * sizeof(struct vector_node));
    if (vector->array == NULL) {
        fprintf(stderr, "Could not reallocate memory for vector array.\n");
        exit(1);
    }

    // Initialize the new nodes
    for (int i = oldSize; i < vector->size; i++) {
        vector->array[i].data = NULL;
    }

    // Add the new node
    vector->array[oldSize].data = data;
}

// Get the size of the vector
int getVectorSize(Vector vector) {
    return vector->size;
}

// Get the data at a given index
void *getVectorData(Vector vector, int index) {
    if (index < 0 || index >= vector->size) {
        fprintf(stderr, "Index out of bounds.\n");
        return NULL;
    }
    return vector->array[index].data;
}

void *findVectorData(Vector vector, void *data, bool (*compare)(void *, void *)) {
    for (int i = 0; i < vector->size; i++) {
        if (compare(vector->array[i].data, data)) {
            return vector->array[i].data;
        }
    }
    return NULL;
}

// Free the vector
void freeVector(Vector vector) {
    free(vector->array);
    free(vector);
}
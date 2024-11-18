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
    vector->count = 0; // Initialize count to 0
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
    // Check if the vector is full
    if (vector->count == vector->size) {
        // Double the size of the vector
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
    }

    // Add the new node
    vector->array[vector->count].data = data;
    vector->count++; // Increment the count
}

// Get the size of the vector
int getVectorSize(Vector vector) {
    return vector->count; // Return the count of elements
}

// Get the data at a given index
void *getVectorData(Vector vector, int index) {
    if (index < 0 || index >= vector->count) { // Check against count
        fprintf(stderr, "Index out of bounds.\n");
        return NULL;
    }
    return vector->array[index].data;
}

// Set the data at a given index
void setVectorData(Vector vector, int index, void *data) {
    if (index < 0 || index >= vector->count) { // Check against count
        fprintf(stderr, "Index out of bounds.\n");
        return;
    }
    vector->array[index].data = data;
}

// Find data in the vector
void *findVectorData(Vector vector, void *data, bool (*compare)(void *, void *)) {
    for (int i = 0; i < vector->count; i++) { // Iterate up to count
        if (compare(vector->array[i].data, data)) {
            return vector->array[i].data;
        }
    }
    return NULL;
}

// Sort the vector
void sortVector(Vector vector, int (*compare)(const void *, const void *)) {
    for (int i = 0; i < vector->count; i++) {
        for (int j = i + 1; j < vector->count; j++) {
            if (compare(vector->array[i].data, vector->array[j].data) > 0) {
                void *temp = vector->array[i].data;
                vector->array[i].data = vector->array[j].data;
                vector->array[j].data = temp;
            }
        }
    }
}

// Free the vector
void freeVector(Vector vector) {
    free(vector->array);
    free(vector);
}
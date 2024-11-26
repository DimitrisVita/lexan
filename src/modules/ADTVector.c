#include "ADTVector.h"

// Create a new vector
Vector createVector(int size) {
    Vector vector = (Vector)malloc(sizeof(struct vector));
    if (vector == NULL) {
        fprintf(stderr, "Could not allocate memory for vector.\n");
        exit(1);
    }

    vector->size = size;
    vector->count = 0;
    vector->array = (VectorNode)malloc(size * sizeof(struct vector_node));
    if (vector->array == NULL) {
        fprintf(stderr, "Could not allocate memory for vector array.\n");
        free(vector);
        exit(1);
    }

    // Initialize the data pointers to NULL
    for (int i = 0; i < size; i++)
        vector->array[i].data = NULL;

    return vector;
}

// Add a new node to the vector 
void addVectorNode(Vector vector, void *data) {
    if (vector->count == vector->size) {
        int oldSize = vector->size;
        vector->size *= 2;
        vector->array = (VectorNode)realloc(vector->array, vector->size * sizeof(struct vector_node));
        if (vector->array == NULL) {
            fprintf(stderr, "Could not reallocate memory for vector array.\n");
            exit(1);
        }

        // Initialize the new data pointers to NULL
        for (int i = oldSize; i < vector->size; i++) {
            vector->array[i].data = NULL;
        }
    }

    vector->array[vector->count].data = data;
    vector->count++;
}

// Get the size of the vector
int getVectorSize(Vector vector) {
    return vector->count;
}

// Get the data at a given index
void *getVectorData(Vector vector, int index) {
    if (index < 0 || index >= vector->count) {
        fprintf(stderr, "Index out of bounds.\n");
        return NULL;
    }
    return vector->array[index].data;
}

// Set the data at a given index
void setVectorData(Vector vector, int index, void *data) {
    if (index < 0 || index >= vector->count) {
        fprintf(stderr, "Index out of bounds.\n");
        return;
    }
    vector->array[index].data = data;
}

// Find data in the vector
void *findVectorData(Vector vector, void *data, bool (*compare)(void *, void *)) {
    for (int i = 0; i < vector->count; i++)
        if (compare(vector->array[i].data, data))
            return vector->array[i].data;
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
void freeVector(Vector vector, void (*freeData)(void *)) {
    for (int i = 0; i < vector->count; i++)
        freeData(vector->array[i].data);
    free(vector->array);
    free(vector);
}
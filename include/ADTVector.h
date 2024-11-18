#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct vector* Vector;
typedef struct vector_node* VectorNode;

struct vector_node {
    void *data;
};

struct vector {
    int size;
    int count;
    VectorNode array;
};

// Create a new vector
Vector createVector(int size);

// Add a new node to the vector
void addVectorNode(Vector vector, void *data);

// Get the size of the vector
int getVectorSize(Vector vector);

// Get the data at a given index
void *getVectorData(Vector vector, int index);

// Set the data at a given index
void setVectorData(Vector vector, int index, void *data);

// Find data in the vector
void *findVectorData(Vector vector, void *data, bool (*compare)(void *, void *));

// Sort the vector
void sortVector(Vector vector, int (*compare)(const void *, const void *));

// Free the vector
void freeVector(Vector vector);
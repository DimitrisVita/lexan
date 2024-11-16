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

void *findVectorData(Vector vector, void *data, bool (*compare)(void *, void *));

// Free the vector
void freeVector(Vector vector);
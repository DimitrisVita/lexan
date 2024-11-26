#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <getopt.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <sys/times.h>
#include <stdbool.h>

typedef struct {
    char *word;
    int count;
} Word;

void freeWord(void *word);

int compareWords(const void *a, const void *b);

ssize_t safeRead(int fd, void *buffer, size_t count);
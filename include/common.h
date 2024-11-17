#ifndef COMMON_H
#define COMMON_H

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

typedef struct {
    char *word;
    int count;
} Word;

ssize_t safeRead(int fd, void *buffer, size_t count);

void sendTimeToRoot(double time_spent);



#endif // COMMON_H
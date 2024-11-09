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


void sendTimeToRoot(double time_spent);

#endif // COMMON_H
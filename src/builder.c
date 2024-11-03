#include "common.h"

int main(int argc, char *argv[]) {

    kill(getppid(), SIGUSR2);

    return 0;
}
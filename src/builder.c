#include "common.h"

int main(int argc, char *argv[]) {
    // Λήψη λέξεων από splitters μέσω pipes
    // Μέτρηση συχνότητας λέξεων

    // Αποστολή αποτελεσμάτων στον root μέσω pipe
    // Αποστολή σήματος USR2 στον root
    kill(getppid(), SIGUSR2);

    return 0;
}
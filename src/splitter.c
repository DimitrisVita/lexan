#include "common.h"

int main(int argc, char *argv[]) {
    // Ανάγνωση τμήματος κειμένου
    // Διαχωρισμός λέξεων και αποστολή στους builders μέσω pipes

    // Αποστολή σήματος USR1 στον root
    kill(getppid(), SIGUSR1);

    return 0;
}
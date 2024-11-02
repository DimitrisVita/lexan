#include "common.h"

void handle_usr1(int sig) {
    // Διαχείριση σήματος USR1
}

void handle_usr2(int sig) {
    // Διαχείριση σήματος USR2
}

int main(int argc, char *argv[]) {
    // Ανάγνωση παραμέτρων γραμμής εντολών
    // Δημιουργία splitters και builders με fork()
    // Άνοιγμα pipes για επικοινωνία

    signal(SIGUSR1, handle_usr1);
    signal(SIGUSR2, handle_usr2);

    // Αναμονή για ολοκλήρωση splitters και builders
    // Συλλογή και επεξεργασία αποτελεσμάτων

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

void handle_usr1(int sig) {
    printf("Received SIGUSR1\n");
}

void handle_usr2(int sig) {
    printf("Received SIGUSR2\n");
}

int main() {
    // Ορισμός συναρτήσεων χειρισμού σημάτων
    signal(SIGUSR1, handle_usr1);
    signal(SIGUSR2, handle_usr2);

    // Δημιουργία νέας διεργασίας
    pid_t pid = fork();

    if (pid < 0) {
        // Σφάλμα κατά τη δημιουργία της διεργασίας
        fprintf(stderr, "Fork failed\n");
        return 1;
    } else if (pid == 0) {
        // Κώδικας που εκτελείται από το παιδί
        printf("Child process: My PID is %d\n", getpid());
        // Αποστολή σήματος SIGUSR1 στον γονέα
        kill(getppid(), SIGUSR1);
        exit(0);
    } else {
        // Κώδικας που εκτελείται από τον γονέα
        printf("Parent process: My PID is %d\n", getpid());
        // Αναμονή για την ολοκλήρωση του παιδιού
        wait(NULL);
    }

    return 0;
}
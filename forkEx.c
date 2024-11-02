#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid;

    // Δημιουργία νέας διεργασίας
    pid = fork();

    if (pid < 0) {
        // Σφάλμα κατά τη δημιουργία της διεργασίας
        fprintf(stderr, "Fork failed\n");
        return 1;
    } else if (pid == 0) {
        // Κώδικας που εκτελείται από το παιδί
        printf("Child process: My PID is %d\n", getpid());
        printf("Child process: My parent's PID is %d\n", getppid());
    } else {
        // Κώδικας που εκτελείται από τον γονέα
        printf("Parent process: My PID is %d\n", getpid());
        printf("Parent process: My child's PID is %d\n", pid);
        // Αναμονή για την ολοκλήρωση του παιδιού
        wait(NULL);
    }

    return 0;
}
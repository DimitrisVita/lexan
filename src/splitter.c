#include "common.h"

int main(int argc, char *argv[]) {
    // Test if the number of arguments is correct
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Open the file using system call open
    // file is in split_files directory
    char *file = argv[1];
    char path[1024];
    snprintf(path, sizeof(path), "split_files/%s", file);
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "Splitter: My PID is %d\n", getpid());
    // Read the file line by line and print it
    char line[1024];
    ssize_t bytesRead;
    while ((bytesRead = read(fd, line, sizeof(line))) > 0) {
        write(STDOUT_FILENO, line, bytesRead);
    }


    // Αποστολή σήματος USR1 στον root
    kill(getppid(), SIGUSR1);

    return 0;
}
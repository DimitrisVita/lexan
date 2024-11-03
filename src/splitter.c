#include "common.h"

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s textFile exclusionList startDesc endDesc\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *textFile = argv[1];
    char *exclusionList = argv[2];
    int startDesc = atoi(argv[3]);
    int endDesc = atoi(argv[4]);

    if (startDesc < 0 || endDesc < -1) {
        fprintf(stderr, "Usage: %s textFile exclusionList startDesc endDesc\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Open text file using file descriptor
    int fd = open(textFile, O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // Open exclusion list file using file descriptor
    int exclusionFd = open(exclusionList, O_RDONLY);
    if (exclusionFd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // Move file descriptor to start line using file descriptor startLine
    lseek(fd, startDesc, SEEK_SET);

    // print lines from startDesc to endDesc
    char c;
    while (read(fd, &c, 1) > 0) {
        if (endDesc != -1 && lseek(fd, 0, SEEK_CUR) > endDesc) {
            break;
        }
        write(STDOUT_FILENO, &c, 1);
    }

    // Close file descriptors
    close(fd);
    close(exclusionFd);

    // Αποστολή σήματος USR1 στον root
    kill(getppid(), SIGUSR1);

    return 0;
}
#include "common.h"
#include "ADTSet.h"

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

    // Open exclusion list file using file descriptor
    char exclusionPath[128];
    sprintf(exclusionPath, "./%s", exclusionList);
    int exclusionFd = open(exclusionPath, O_RDONLY);
    if (exclusionFd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // Create a set to store the exclusion list
    Set exclusionSet = createSet(100);

    // Read the exclusion list file and add each line to the set
    char c;
    char line[128];
    int i = 0;
    while (read(exclusionFd, &c, 1) > 0) {
        if (c == '\n') {
            line[i] = '\0';
            addElement(exclusionSet, line);
            i = 0;
        } else {
            line[i++] = c;
        }
    }

    // Check if set contains a specific element
    printf("Contains element: %d\n", containsElement(exclusionSet, "b"));



    // // Open text file using file descriptor
    // char path[128];
    // sprintf(path, "./%s", textFile); // Ensure the path is correct
    // int textFd = open(path, O_RDONLY);
    // if (textFd == -1) {
    //     perror("open");
    //     exit(EXIT_FAILURE);
    // }


    // // Move file descriptor to start line using file descriptor startLine
    // lseek(textFd, startDesc, SEEK_SET);

    // // print lines from startDesc to endDesc
    // char c;
    // while (read(textFd, &c, 1) > 0) {
    //     if (endDesc != -1 && lseek(textFd, 0, SEEK_CUR) > endDesc) {
    //         break;
    //     }
    //     write(STDOUT_FILENO, &c, 1);
    // }

    // Close file descriptors
    // close(textFd);
    close(exclusionFd);

    // Αποστολή σήματος USR1 στον root
    kill(getppid(), SIGUSR1);

    return 0;
}
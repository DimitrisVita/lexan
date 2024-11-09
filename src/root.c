#include "common.h"

void splitterDone(int sig) {
    // Διαχείριση σήματος USR1
    printf("Splitter done\n");
}

void builderDone(int sig) {
    // Διαχείριση σήματος USR2
    printf("Builder done\n");
}

// Function that save startDescriptor for each splitter in an array
int *saveStartDescriptors(int numOfSplitter, char *textFile) {
    int fd = open(textFile, O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // Count lines of text file using file descriptor
    int lines = 0;
    char c;
    while (read(fd, &c, 1) > 0)
        if (c == '\n')
            lines++;

    // Lines per splitter
    int linesPerSplitter = lines / numOfSplitter;
    int remainingLines = lines % numOfSplitter;

    int *startDescriptors = (int *)malloc(numOfSplitter * sizeof(int)); // Array to save startDescriptor for each splitter
    int characterCount = 0; // Count characters read
    int lineCount = 0;    // Count lines read
    lseek(fd, 0, SEEK_SET);
    for (int i = 0; i < numOfSplitter; i++) {
        startDescriptors[i] = characterCount;
        for (int j = 0; j < linesPerSplitter + (i < remainingLines ? 1 : 0); j++) {
            while (read(fd, &c, 1) > 0) {
                characterCount++;
                if (c == '\n') {
                    lineCount++;
                    break;
                }
            }
        }
    }
    close(fd);
    return startDescriptors;
}



int main(int argc, char *argv[]) {
    ////////////////////////////
    ///// Argument parsing /////
    ////////////////////////////

    char *textFile = NULL;
    int numOfSplitter = 0;
    int numOfBuilders = 0;
    int topPopular = 0; // How many popular words to print
    char *exclusionList = NULL; // File with words to exclude
    char *outputFile = NULL;

    int opt;
    while ((opt = getopt(argc, argv, "t:s:b:p:e:o:")) != -1) {
        switch (opt) {
            case 't':
                textFile = optarg;
                break;
            case 's':
                numOfSplitter = atoi(optarg);
                break;
            case 'b':
                numOfBuilders = atoi(optarg);
                break;
            case 'p':
                topPopular = atoi(optarg);
                break;
            case 'e':
                exclusionList = optarg;
                break;
            case 'o':
                outputFile = optarg;
                break;
            default:
                fprintf(stderr, "Usage: %s -t textFile -s numOfSplitter -b numOfBuilders -p topPopular -e exclusionList -o outputFile\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    
    if (textFile == NULL || numOfSplitter <= 0 || numOfBuilders <= 0 || topPopular <= 0 || exclusionList == NULL || outputFile == NULL) {
        fprintf(stderr, "Usage: %s -t textFile -s numOfSplitter -b numOfBuilders -p topPopular -e exclusionList -o outputFile\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    ///////////////////////////
    ///// Signal handling /////
    ///////////////////////////

    signal(SIGUSR1, splitterDone);
    signal(SIGUSR2, builderDone);

    ///////////////////////////
    ///// Process creation ////
    ///////////////////////////

    // Create pipes
    int pipes[numOfBuilders][2];
    for (int i = 0; i < numOfBuilders; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    // Calculate startDescriptor for each splitter
    int *startDescriptors = saveStartDescriptors(numOfSplitter, textFile);

    // Create splitter processes
    for (int i = 0; i < numOfSplitter; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Calculate start and end line for each splitter
            int startDesc = startDescriptors[i];
            int endDesc = (i == numOfSplitter - 1) ? -1 : startDescriptors[i + 1];

            char startDescStr[10];
            char endDescStr[10];
            sprintf(startDescStr, "%d", startDesc);
            sprintf(endDescStr, "%d", endDesc);

            // Close read ends of pipes in splitter
            for (int j = 0; j < numOfBuilders; j++) {
                close(pipes[j][0]);
            }

            execl("./splitter", "splitter", textFile, exclusionList, startDescStr, endDescStr, (char *)NULL);
            perror("execl");
            exit(EXIT_FAILURE);
        }
    }

    // Create builder processes
    for (int i = 0; i < numOfBuilders; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Close write ends of pipes in builder
            for (int j = 0; j < numOfBuilders; j++) {
                if (i != j) {
                    close(pipes[j][1]);
                }
            }
            dup2(pipes[i][0], STDIN_FILENO); // Redirect input from pipe
            close(pipes[i][0]);
            execl("./builder", "builder", (char *)NULL);
            perror("execl");
            exit(EXIT_FAILURE);
        } else {
            close(pipes[i][0]); // Close read end in parent
        }
    }

    ///////////////////////////
    ///// Waiting for child ///
    ///////////////////////////

    int status;
    for (int i = 0; i < numOfSplitter + numOfBuilders; i++) {
        wait(&status);
    }

    // Free memory
    free(startDescriptors);
    return 0;
}
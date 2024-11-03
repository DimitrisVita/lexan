#include "common.h"

void splitterDone(int sig) {
    // Διαχείριση σήματος USR1
}

void builderDone(int sig) {
    // Διαχείριση σήματος USR2
}

int countLines(const char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        printf("filename: %s\n", filename);
        perror("open");
        exit(EXIT_FAILURE);
    }
    int lines = 0;
    char ch;
    while (read(fd, &ch, 1) == 1) {
        if (ch == '\n') {
            lines++;
        }
    }
    close(fd);
    return lines;
}

void splitFile(const char *filename, int numOfSplitter) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        printf("Το αρχείο %s δεν υπάρχει\n", filename);
        perror("open");
        exit(EXIT_FAILURE);
    }

    int totalLines = countLines(filename);
    int linesPerSplitter = totalLines / numOfSplitter;
    int remainingLines = totalLines % numOfSplitter;

    char line[1024];
    for (int i = 0; i < numOfSplitter; i++) {
        char splitterFilename[256];
        snprintf(splitterFilename, sizeof(splitterFilename), "split_files/splitter_%d.txt", i);
        int splitterFd = open(splitterFilename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (splitterFd == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }

        int linesToWrite = linesPerSplitter + (i < remainingLines ? 1 : 0);
        for (int j = 0; j < linesToWrite; j++) {
            ssize_t bytesRead;
            int lineLength = 0;
            while ((bytesRead = read(fd, &line[lineLength], 1)) > 0 && line[lineLength] != '\n') {
                lineLength++;
            }
            line[lineLength] = '\n';
            write(splitterFd, line, lineLength + 1);
        }
        close(splitterFd);
    }
    close(fd);
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
    ///// File splitting //////
    ///////////////////////////


    splitFile(textFile, numOfSplitter);

    ///////////////////////////
    ///// Process creation ////
    ///////////////////////////

    // Create splitter processes
    for (int i = 0; i < numOfSplitter; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            char splitterFilename[256];
            snprintf(splitterFilename, sizeof(splitterFilename), "splitter_%d.txt", i);

            execl("./splitter", "./splitter", splitterFilename, exclusionList, NULL);
            perror("execl");
            exit(EXIT_FAILURE);
        }
    }

    // // Create builder processes
    // for (int i = 0; i < numOfBuilders; i++) {
    //     pid_t pid = fork();
    //     if (pid < 0) {
    //         perror("fork");
    //         exit(EXIT_FAILURE);
    //     } else if (pid == 0) {
    //         // Εκτέλεση του προγράμματος builder
    //         execl("./builder", "builder", (char *)NULL);
    //         perror("execl");
    //         exit(EXIT_FAILURE);
    //     }
    // }

    wait(NULL);
        

    return 0;
}
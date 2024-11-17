#include "common.h"
#include "ADTVector.h"

void splitterDone(int sig) {
    // Διαχείριση σήματος USR1
    // printf("Splitter done\n");
}

void builderDone(int sig) {
    // Διαχείριση σήματος USR2
    // printf("Builder done\n");
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

    // Create pipes for splitter-builder communication
    int SBpipes[numOfBuilders][2];
    for (int i = 0; i < numOfBuilders; i++) {
        if (pipe(SBpipes[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    // Create pipes from builders to root
    int BRpipes[numOfBuilders][2];
    for (int i = 0; i < numOfBuilders; i++) {
        if (pipe(BRpipes[i]) == -1) {
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
            char numOfBuildersStr[10];
            sprintf(startDescStr, "%d", startDesc);
            sprintf(endDescStr, "%d", endDesc);
            sprintf(numOfBuildersStr, "%d", numOfBuilders);

            // Close read ends of pipes in splitter
            for (int j = 0; j < numOfBuilders; j++) {
                close(SBpipes[j][0]);
            }

            // Concatenate pipe file descriptors into a single string
            char pipeDescriptors[1024] = ""; // Ensure this buffer is large enough to hold all descriptors
            for (int j = 0; j < numOfBuilders; j++) {
                char SBpipestr[10];
                sprintf(SBpipestr, "%d", SBpipes[j][1]);
                strcat(pipeDescriptors, SBpipestr);
                if (j < numOfBuilders - 1) {
                    strcat(pipeDescriptors, ",");
                }
            }

            // Create argument list for execl
            execl("./splitter", "splitter", textFile, exclusionList, startDescStr, endDescStr, numOfBuildersStr, pipeDescriptors, (char *)NULL);
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
                close(SBpipes[j][1]);
            }
            // Close read ends of BRpipes in builder
            for (int j = 0; j < numOfBuilders; j++) {
                close(BRpipes[j][0]);
            }
            dup2(BRpipes[i][1], STDOUT_FILENO); // Redirect output to BRpipe
            close(BRpipes[i][1]);
            dup2(SBpipes[i][0], STDIN_FILENO); // Redirect input from pipe
            close(SBpipes[i][0]);
            execl("./builder", "builder", (char *)NULL);
            perror("execl");
            exit(EXIT_FAILURE);
        } else {
            close(SBpipes[i][0]); // Close read end in parent
            close(SBpipes[i][1]); // Close write end in parent
            close(BRpipes[i][1]); // Close write end in parent
        }
    }

    ///////////////////////////
    ///// Waiting for child ///
    ///////////////////////////

    int status;
    for (int i = 0; i < numOfSplitter + numOfBuilders; i++) {
        wait(&status);
    }

    ///////////////////////////
    ///// Reading from pipes //
    ///////////////////////////

    // Create vector to store words structs
    Vector words = createVector(1000);

    // Read from pipes
    for (int i = 0; i < numOfBuilders; i++) {
        char buffer[256];
        char leftover[256] = {0}; // Buffer to store leftover data
        int leftoverLen = 0;
        int bytesRead;

        while ((bytesRead = safeRead(BRpipes[i][0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytesRead] = '\0';

            // print in file
            FILE *f = fopen(outputFile, "a");
            if (f == NULL) {
                printf("Error opening file!\n");
                exit(1);
            }

            fprintf(f, "%s", buffer);

            fclose(f);

            // Prepend leftover to the buffer
            char combinedBuffer[512];
            int combinedLen = leftoverLen + bytesRead;
            memcpy(combinedBuffer, leftover, leftoverLen);
            memcpy(combinedBuffer + leftoverLen, buffer, bytesRead);
            combinedBuffer[combinedLen] = '\0';

            char *line = combinedBuffer;
            char *newline;
            while ((newline = strchr(line, '\n')) != NULL) {
                *newline = '\0';

                // Check if the line contains a complete word-count pair
                char *delimiter = strchr(line, '*');
                if (delimiter != NULL) {
                    // Extract the word
                    *delimiter = '\0';
                    char *word = line;

                    // Extract the count
                    char *countStr = delimiter + 1;
                    int count = atoi(countStr);

                    // Create word struct and add it to vector
                    Word *wordStruct = (Word *)malloc(sizeof(Word));
                    wordStruct->word = strdup(word);
                    wordStruct->count = count;

                    addVectorNode(words, wordStruct);
                } else {
                    // Save the incomplete line as leftover
                    leftoverLen = strlen(line);
                    memcpy(leftover, line, leftoverLen);
                    leftover[leftoverLen] = '\0';
                }

                line = newline + 1;
            }

            // Save any remaining incomplete line as leftover
            leftoverLen = strlen(line);
            memcpy(leftover, line, leftoverLen);
            leftover[leftoverLen] = '\0';
        }
        close(BRpipes[i][0]);
    }

    // Print vector's data

    printf("Vector size: %d\n", getVectorSize(words));

    // Sort vector
    for (int i = 0; i < getVectorSize(words); i++) {
        for (int j = i + 1; j < getVectorSize(words); j++) {
            Word *word1 = (Word *)getVectorData(words, i);
            Word *word2 = (Word *)getVectorData(words, j);
            if (word1->count < word2->count) {
                void *temp = getVectorData(words, i);
                setVectorData(words, i, getVectorData(words, j));
                setVectorData(words, j, temp);
            }
        }
    }

    // Print top popular words
    for (int i = 0; i < topPopular && i < getVectorSize(words); i++) {
        Word *word = (Word *)getVectorData(words, i);
        printf("%s: %d\n", word->word, word->count);
    }

    // Free memory
    free(startDescriptors);
    return 0;
}
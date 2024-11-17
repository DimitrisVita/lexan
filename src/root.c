#include "common.h"
#include "ADTVector.h"

// Global variables to count signals received
volatile sig_atomic_t usr1_count = 0;
volatile sig_atomic_t usr2_count = 0;

// Signal handlers
void splitterDone(int sig) { usr1_count++; }
void builderDone(int sig) { usr2_count++; }

// Function for argument parsing
void argumentParsing(int argc, char *argv[], char **textFile, int *numOfSplitter, int *numOfBuilders, int *topPopular, char **exclusionList, char **outputFile) {
    int opt;
    while ((opt = getopt(argc, argv, "t:s:b:p:e:o:")) != -1) {
        switch (opt) {
            case 't':
                *textFile = optarg;
                break;
            case 's':
                *numOfSplitter = atoi(optarg);
                break;
            case 'b':
                *numOfBuilders = atoi(optarg);
                break;
            case 'p':
                *topPopular = atoi(optarg);
                break;
            case 'e':
                *exclusionList = optarg;
                break;
            case 'o':
                *outputFile = optarg;
                break;
            default:
                fprintf(stderr, "Usage: %s -t textFile -s numOfSplitter -b numOfBuilders -p topPopular -e exclusionList -o outputFile\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    
    if (*textFile == NULL || *numOfSplitter <= 0 || *numOfBuilders <= 0 || *topPopular <= 0 || *exclusionList == NULL || *outputFile == NULL) {
        fprintf(stderr, "Usage: %s -t textFile -s numOfSplitter -b numOfBuilders -p topPopular -e exclusionList -o outputFile\n", argv[0]);
        exit(EXIT_FAILURE);
    }
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

    int *startDescriptors = (int *)malloc(numOfSplitter * sizeof(int)); // Array to save startDescriptor
    int readCount = 0;  // Count characters read
    int lineCount = 0;  // Count lines read

    lseek(fd, 0, SEEK_SET); // Reset file descriptor to start of file
    
    // Save startDescriptor for each splitter
    for (int i = 0; i < numOfSplitter; i++) {
        startDescriptors[i] = readCount;
        for (int j = 0; j < linesPerSplitter + (i < remainingLines ? 1 : 0); j++) {
            while (read(fd, &c, 1) > 0) {
                readCount++;
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

// Function to create pipes needed for communication
void createPipes(int numOfPipes, int pipes[numOfPipes][2]) {
    for (int i = 0; i < numOfPipes; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }
}

// Function to create splitter processes
void createSplitterProcesses(int numOfSplitter, int numOfBuilders, int startDescriptors[], char *textFile, char *exclusionList, int SBpipes[numOfBuilders][2]) {
    for (int i = 0; i < numOfSplitter; i++) {
        pid_t pid = fork();
        if (pid < 0) {  // Error
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {  // Child
            char startDescStr[10], endDescStr[10], numOfBuildersStr[10];
            sprintf(startDescStr, "%d", startDescriptors[i]);
            sprintf(endDescStr, "%d", (i == numOfSplitter - 1) ? -1 : startDescriptors[i + 1]);
            sprintf(numOfBuildersStr, "%d", numOfBuilders);

            // Concatenate pipe file descriptors into a single string
            char *pipeDescriptors = (char *)malloc(10 * numOfBuilders);
            for (int j = 0; j < numOfBuilders; j++) {
                char SBpipestr[10];
                sprintf(SBpipestr, "%d", SBpipes[j][1]);
                strcat(pipeDescriptors, SBpipestr);
                if (j < numOfBuilders - 1)
                    strcat(pipeDescriptors, ",");
            }

            // Close write ends of pipes in splitter
            for (int j = 0; j < numOfBuilders; j++)
                close(SBpipes[j][0]);

            // Execute splitter
            execl("./splitter", "splitter", textFile, exclusionList, startDescStr, endDescStr, numOfBuildersStr, pipeDescriptors, (char *)NULL);
            free(pipeDescriptors);  // Free memory
            perror("execl");
            exit(EXIT_FAILURE);
        }
    }
}

// Function to create builder processes
void createBuilderProcesses(int numOfBuilders, int SBpipes[numOfBuilders][2], int BRpipes[numOfBuilders][2]) {
    for (int i = 0; i < numOfBuilders; i++) {
        pid_t pid = fork();
        if (pid < 0) {  // Error
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {  // Child
            // Close write ends of SBpipes in builder
            for (int j = 0; j < numOfBuilders; j++)
                close(SBpipes[j][1]);

            // Close read ends of BRpipes in builder
            for (int j = 0; j < numOfBuilders; j++)
                close(BRpipes[j][0]);

            // Redirect output to BRpipe
            dup2(BRpipes[i][1], STDOUT_FILENO);
            close(BRpipes[i][1]);

            // Redirect input from SBpipe
            dup2(SBpipes[i][0], STDIN_FILENO);
            close(SBpipes[i][0]);

            // Execute builder
            execl("./builder", "builder", (char *)NULL);
            perror("execl");
            exit(EXIT_FAILURE);
        } else {
            close(SBpipes[i][0]);  // Close read end in parent
            close(SBpipes[i][1]);  // Close write end in parent
            close(BRpipes[i][1]);  // Close write end in parent
        }
    }
}

int main(int argc, char *argv[]) {
    // Start timer for root process
    struct tms tb1, tb2;
    double t1, t2, real_time, ticspersec = (double) sysconf(_SC_CLK_TCK);
    t1 = (double) times(&tb1);

    // Parse arguments
    char *textFile = NULL, *exclusionList = NULL, *outputFile = NULL;
    int numOfSplitter = 0, numOfBuilders = 0, topPopular = 0;
    argumentParsing(argc, argv, &textFile, &numOfSplitter, &numOfBuilders, &topPopular, &exclusionList, &outputFile);
    
    // Signal handling
    signal(SIGUSR1, splitterDone);
    signal(SIGUSR2, builderDone);

    // Create pipes splitters-builders and builders-root
    int SBpipes[numOfBuilders][2], BRpipes[numOfBuilders][2];
    createPipes(numOfBuilders, SBpipes);
    createPipes(numOfBuilders, BRpipes);

    // Calculate startDescriptor for each splitter
    int *startDescriptors = saveStartDescriptors(numOfSplitter, textFile);    

    // Create splitter and builder processes
    createSplitterProcesses(numOfSplitter, numOfBuilders, startDescriptors, textFile, exclusionList, SBpipes);
    createBuilderProcesses(numOfBuilders, SBpipes, BRpipes);    

    // Wait for all children to finish
    int status;
    for (int i = 0; i < numOfSplitter + numOfBuilders; i++)
        wait(&status);

    // Create vector to store words structs
    Vector words = createVector(1000);

    printf("BUILDERS TIME:\n");

    // Read from pipes
    for (int i = 0; i < numOfBuilders; i++) {
        char buffer[256];
        char leftover[256] = {0}; // Buffer to store leftover data
        int leftoverLen = 0;
        int bytesRead;

        while ((bytesRead = safeRead(BRpipes[i][0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytesRead] = '\0';

            if (bytesRead == -1) {
                perror("read");
                exit(EXIT_FAILURE);
            }


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

                // Check if the line contains CPU time information
                if (strncmp(line, "TIME:", 5) == 0) {
                    double cpu_time = atof(line + 5);
                    printf("%lf sec\n", cpu_time);
                } else {
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

    ///////////////////////////
    ///// Writing to file /////
    ///////////////////////////

    int fd = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < getVectorSize(words); i++) {
        Word *word = (Word *)getVectorData(words, i);
        char line[256];
        snprintf(line, sizeof(line), "%s: %d\n", word->word, word->count);
        write(fd, line, strlen(line));
    }

    close(fd);

    // End timing
    t2 = (double) times(&tb2);
    real_time = (t2 - t1) / ticspersec;

    printf("ROOT TIME:\n");
    printf("%lf sec\n", real_time);

    // Print top popular words
    printf("TOP %d POPULAR WORDS:\n", topPopular);
    for (int i = 0; i < topPopular && i < getVectorSize(words); i++) {
        Word *word = (Word *)getVectorData(words, i);
        printf("%s: %d\n", word->word, word->count);
    }

    // Print the number of signals received
    printf("Number of SIGUSR1 signals received: %d\n", usr1_count);
    printf("Number of SIGUSR2 signals received: %d\n", usr2_count);

    // Free memory
    free(startDescriptors);
    return 0;
}
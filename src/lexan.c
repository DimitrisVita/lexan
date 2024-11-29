#include "common.h"
#include "ADTVector.h"

// Global variables to count signals received
int usr1_count = 0;
int usr2_count = 0;

// Global variable to count total number of interesting words
int totalInterestingWords = 0;

// Signal handlers
void splitterDone(int sig) { usr1_count++; }
void builderDone(int sig) { usr2_count++; }

// Function for argument parsing
void argumentParsing(int argc, char *argv[], char **textFile, int *numOfSplitter, int *numOfBuilders, int *topPopular, char **exclusionList, char **outputFile) {
    int opt;
    while ((opt = getopt(argc, argv, "i:l:m:t:e:o:")) != -1) {
        switch (opt) {
            case 'i': *textFile = optarg; break;
            case 'l': *numOfSplitter = atoi(optarg); break;
            case 'm': *numOfBuilders = atoi(optarg); break;
            case 't': *topPopular = atoi(optarg); break;
            case 'e': *exclusionList = optarg; break;
            case 'o': *outputFile = optarg; break;
            default:
                fprintf(stderr, "Usage: %s -i textFile -l numOfSplitter -m numOfBuilders -t topPopular -e exclusionList -o outputFile\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    
    if (*textFile == NULL || *numOfSplitter <= 0 || *numOfBuilders <= 0 || *topPopular <= 0 || *exclusionList == NULL || *outputFile == NULL) {
        fprintf(stderr, "Usage: %s -i textFile -l numOfSplitter -m numOfBuilders -t topPopular -e exclusionList -o outputFile\n", argv[0]);
        exit(EXIT_FAILURE);
    }
}

// Function that saves startDescriptor for each splitter in an array
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

    lseek(fd, 0, SEEK_SET); // Reset file descriptor to start of file
    
    // Save startDescriptor for each splitter
    for (int i = 0; i < numOfSplitter; i++) {
        startDescriptors[i] = readCount;
        for (int j = 0; j < linesPerSplitter + (i < remainingLines ? 1 : 0); j++) {
            while (read(fd, &c, 1) > 0) {
                readCount++;
                if (c == '\n') break;
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
            pipeDescriptors[0] = '\0'; // Initialize the string
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
            execl("./obj/splitter", "splitter", textFile, exclusionList, startDescStr, endDescStr, numOfBuildersStr, pipeDescriptors, (char *)NULL);
            free(pipeDescriptors);  // Free memory
            perror("execl");
            exit(EXIT_FAILURE);
        }
    }
    free(startDescriptors);  // Free memory
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
            execl("./obj/builder", "builder", (char *)NULL);
            perror("execl");
            exit(EXIT_FAILURE);
        } else {
            close(SBpipes[i][0]);  // Close read end in parent
            close(SBpipes[i][1]);  // Close write end in parent
            close(BRpipes[i][1]);  // Close write end in parent
        }
    }
}

// Function to read from pipe
void readFromPipe(int fd, Vector words, double builderTimes[], int *builderIndex) {
    char word[128]; // Buffer to store word
    int wordLen;    // Length of word
    while (safeRead(fd, &wordLen, sizeof(int)) > 0) {   // Read word length
        if (wordLen == 777) {   // Length of 777 indicates time spent by builder read
            double time_spent;
            if (safeRead(fd, &time_spent, sizeof(double)) != sizeof(double)) {  // Read time spent
                fprintf(stderr, "Failed to read time\n");
                exit(EXIT_FAILURE);
            }
            builderTimes[(*builderIndex)++] = time_spent;
            continue;
        }

        // Read word length
        if (wordLen <= 0 || wordLen >= sizeof(word)) {
            fprintf(stderr, "Invalid word length: %d\n", wordLen);
            exit(EXIT_FAILURE);
        }

        // Read word
        if (safeRead(fd, word, wordLen) != wordLen) {
            fprintf(stderr, "Failed to read the complete word\n");
            exit(EXIT_FAILURE);
        }
        word[wordLen] = '\0';

        // Read count
        int count;
        if (safeRead(fd, &count, sizeof(int)) != sizeof(int)) {
            fprintf(stderr, "Failed to read the complete count\n");
            exit(EXIT_FAILURE);
        }

        // Add word to vector
        Word *wordStruct = (Word *)malloc(sizeof(Word));
        wordStruct->word = strdup(word);
        wordStruct->count = count;
        addVectorNode(words, wordStruct);

        // Update total number of interesting words
        totalInterestingWords += count;
    }
}

// Function to write words to file
void writeToFile(Vector words, char *outputFile) {
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

    // Array to store time spent by each builder
    double builderTimes[numOfBuilders];
    int builderIndex = 0;

    // Read from pipes
    for (int i = 0; i < numOfBuilders; i++) {
        readFromPipe(BRpipes[i][0], words, builderTimes, &builderIndex);
        close(BRpipes[i][0]);   // Close the pipe after reading
    }

    // Sort vector
    sortVector(words, compareWords);

    // Write words to file
    writeToFile(words, outputFile);
    
    // End timing
    t2 = (double) times(&tb2);
    real_time = (t2 - t1) / ticspersec;

    // Print builder times
    printf("BUILDERS TIME:\n");
    for (int i = 0; i < numOfBuilders; i++)
        printf("%lf sec\n", builderTimes[i]);

    // Print root time
    printf("ROOT TIME:\n");
    printf("%lf sec\n", real_time);

    // Print top popular words with their frequency
    printf("TOP %d POPULAR WORDS:\n", topPopular);
    for (int i = 0; i < topPopular && i < getVectorSize(words); i++) {
        Word *word = (Word *)getVectorData(words, i);
        double frequency = (double)word->count / totalInterestingWords;
        printf("%s: %d (f=%.6lf)\n", word->word, word->count, frequency);
    }

    // Print the number of signals received
    printf("Number of SIGUSR1 signals received: %d\n", usr1_count);
    printf("Number of SIGUSR2 signals received: %d\n", usr2_count);

    // Free memory
    freeVector(words, freeWord);
    return 0;
}
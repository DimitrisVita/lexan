#include "common.h"
#include "ADTSet.h"

// Hash function for sending words to the correct builder process
int hash(char *word, int numBuilders) {
    int sum = 0;
    for (int i = 0; word[i] != '\0'; i++)
        sum += word[i];
    return sum % numBuilders;
}

// Read the exclusion list file and add each line to the set
void readExclusionList(int exclusionFd, Set exclusionSet) {
    char buffer[4096];
    int bytesRead;
    char line[128];
    int i = 0;

    while ((bytesRead = safeRead(exclusionFd, buffer, sizeof(buffer))) > 0) {
        for (int j = 0; j < bytesRead; j++) {
            if (buffer[j] == '\n') {
                line[i] = '\0';
                addElement(exclusionSet, line);
                i = 0;
            } else {
                line[i++] = buffer[j];
            }
        }
    }
}

// Send a word to the correct builder process
void sendWordToBuilder(char *word, int *pipes, int numOfBuilders) {
    int builderIndex = hash(word, numOfBuilders);   // Get the builder index
    int wordLen = strlen(word);
    char message[132];
    memcpy(message, &wordLen, sizeof(int));
    memcpy(message + sizeof(int), word, wordLen);
    write(pipes[builderIndex], message, sizeof(int) + wordLen);
}

// Process the text file and send each word to the correct builder process
void processTextFile(int textFd, Set exclusionSet, int *pipes, int numOfBuilders, int endDesc) {
    char buffer[4096], word[128];
    int wordIndex = 0, bytesRead, currentPos = lseek(textFd, 0, SEEK_CUR);

    while ((bytesRead = safeRead(textFd, buffer, sizeof(buffer))) > 0) {
        for (int i = 0; i < bytesRead; i++, currentPos++) {
            char c = buffer[i];
            if (isalpha(c)) {
                word[wordIndex++] = tolower(c);
            } else if (wordIndex > 0) { // Send the word to the correct builder process
                word[wordIndex] = '\0';
                if (!containsElement(exclusionSet, word))   // Check if the word is in the exclusion list
                    sendWordToBuilder(word, pipes, numOfBuilders);
                wordIndex = 0;
            }
            if (endDesc != -1 && currentPos >= endDesc) break;  // Stop reading if endDesc is reached
        }
        if (endDesc != -1 && currentPos >= endDesc) break;  // Stop reading if endDesc is reached
    }
    if (wordIndex > 0) {    // Send the last word to the correct builder process
        word[wordIndex] = '\0';
        if (!containsElement(exclusionSet, word))
            sendWordToBuilder(word, pipes, numOfBuilders);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 7) {
        fprintf(stderr, "Usage: %s textFile exclusionList startDesc endDesc numOfBuilders pipeDescriptors\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *textFile = argv[1], *exclusionList = argv[2], *pipeDescriptors = argv[6];
    int startDesc = atoi(argv[3]), endDesc = atoi(argv[4]), numOfBuilders = atoi(argv[5]);

    if (startDesc < 0 || endDesc < -1) {
        fprintf(stderr, "Usage: %s textFile exclusionList startDesc endDesc numOfBuilders pipeDescriptors\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Put pipe descriptors in an array
    char *pipeDescriptorsCopy = strdup(pipeDescriptors);
    char *pipeDescriptor;
    int pipes[numOfBuilders];
    int index = 0;

    // Split the pipe descriptors by comma
    while ((pipeDescriptor = strsep(&pipeDescriptorsCopy, ",")) != NULL) {
        pipes[index++] = atoi(pipeDescriptor);
    }
    free(pipeDescriptorsCopy);

    // Open exclusion list file using file descriptor
    char exclusionPath[128];
    sprintf(exclusionPath, "./%s", exclusionList);
    int exclusionFd = open(exclusionPath, O_RDONLY);
    if (exclusionFd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // Create a set to store the exclusion list
    Set exclusionSet = createSet(1000);

    // Read the exclusion list file and add each line to the set
    readExclusionList(exclusionFd, exclusionSet);

    // Open text file using file descriptor
    char path[128];
    sprintf(path, "./%s", textFile);
    int textFd = open(path, O_RDONLY);
    if (textFd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // Move file descriptor to start line using file descriptor startLine
    lseek(textFd, startDesc, SEEK_SET);

    // Process the text file
    processTextFile(textFd, exclusionSet, pipes, numOfBuilders, endDesc);

    // Close pipe descriptors
    for (int i = 0; i < numOfBuilders; i++)
        close(pipes[i]);

    // Free the exclusion set
    freeSet(exclusionSet);

    // Close file descriptors
    close(textFd);
    close(exclusionFd);

    // Send USR1 signal to the parent process
    kill(getppid(), SIGUSR1);

    return 0;
}
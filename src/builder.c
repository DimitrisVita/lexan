#include "common.h"
#include "ADTMap.h"

// Function to read a word from stdin
bool readWord(int fd, char *word, int *wordLen) {
    if (safeRead(fd, wordLen, sizeof(int)) <= 0) return false;
    if (*wordLen <= 0 || *wordLen >= 128) {
        fprintf(stderr, "Invalid word length: %d\n", *wordLen);
        return false;
    }
    if (safeRead(fd, word, *wordLen) != *wordLen) {
        fprintf(stderr, "Failed to read word\n");
        return false;
    }
    word[*wordLen] = '\0';
    return true;
}

// Function to update the word count in the map
void updateWordCount(Map map, char *word) {
    char *value = getMapValue(map, word);
    if (value == NULL) {
        addMapNode(map, strdup(word), strdup("1"));
    } else {
        int count = atoi(value) + 1;
        char countStr[12];
        sprintf(countStr, "%d", count);
        deleteMapNode(map, word);
        addMapNode(map, strdup(word), strdup(countStr));
        free(value); // Free the old value
    }
}

// Function to send word and count to stdout
void sendWordCount(int fd, const char *word, int count) {
    int wordLen = strlen(word);
    char message[140];
    memcpy(message, &wordLen, sizeof(int));
    memcpy(message + sizeof(int), word, wordLen);
    memcpy(message + sizeof(int) + wordLen, &count, sizeof(int));
    write(fd, message, sizeof(int) + wordLen + sizeof(int));
}

void sendTime(int fd, double time) {
    int timeFlag = 777;
    char message[sizeof(int) + sizeof(double)];
    memcpy(message, &timeFlag, sizeof(int));
    memcpy(message + sizeof(int), &time, sizeof(double));
    write(fd, message, sizeof(message));
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s mapSize\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int mapSize = atoi(argv[1]);
    if (mapSize <= 0) {
        fprintf(stderr, "Invalid map size: %d\n", mapSize);
        exit(EXIT_FAILURE);
    }

    // Start timing
    struct tms tb1, tb2;
    double t1, t2, real_time;
    double ticspersec = (double) sysconf(_SC_CLK_TCK);
    t1 = (double) times(&tb1);

    // Read words from stdin and count their occurrences
    Map map = createMap(mapSize);
    char word[128];
    int wordLen;
    while (readWord(STDIN_FILENO, word, &wordLen))
        updateWordCount(map, word);

    // Send words and their counts to root
    for (MapNode node = getFirstMapNode(map); node != NULL; node = getNextMapNode(map, node)) {
        if (node->key && node->value) {
            sendWordCount(STDOUT_FILENO, node->key, atoi(node->value));
        } else {
            fprintf(stderr, "Invalid node key or value\n");
            exit(EXIT_FAILURE);
        }
    }

    close(STDIN_FILENO);

    // End timing
    t2 = (double) times(&tb2);
    real_time = (t2 - t1) / ticspersec;

    // Send real time to root
    sendTime(STDOUT_FILENO, real_time);

    freeMap(map); // Ensure this function frees all nodes and their keys/values

    // Send signal USR2 to root
    kill(getppid(), SIGUSR2);
    return 0;
}
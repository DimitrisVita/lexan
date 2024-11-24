#include "common.h"
#include "ADTMap.h"

int main(int argc, char *argv[]) {
    // Start timing
    struct tms tb1, tb2;
    double t1, t2, real_time;
    double ticspersec = (double) sysconf(_SC_CLK_TCK);
    t1 = (double) times(&tb1);

    // Read words from stdin using read() and count their occurrences
    Map map = createMap(1000);
    char word[128];
    int wordIndex = 0;
    char c;
    int wordLen;
    while (safeRead(STDIN_FILENO, &wordLen, sizeof(int)) > 0) {
        if (wordLen <= 0 || wordLen >= sizeof(word)) {
            fprintf(stderr, "Invalid word length: %d\n", wordLen);
            continue;
        }
        if (safeRead(STDIN_FILENO, word, wordLen) != wordLen) {
            fprintf(stderr, "Failed to read the complete word\n");
            continue;
        }
        word[wordLen] = '\0';        
        char *value = getMapValue(map, word);
        if (value == NULL) {
            addMapNode(map, strdup(word), strdup("1"));
        } else {
            int count = atoi(value);
            count++;
            char countStr[12];
            sprintf(countStr, "%d", count);
            deleteMapNode(map, word);
            addMapNode(map, strdup(word), strdup(countStr));
        }
    }

    // Send words and their counts to root
    for (MapNode node = getFirstMapNode(map); node != NULL; node = getNextMapNode(map, node)) {
        if (node->key == NULL || node->value == NULL) {
            fprintf(stderr, "Invalid node key or value\n");
            continue;
        }
        char *word = node->key;
        int count = atoi(node->value);
        int wordLen = strlen(word);

        char message[140]; // 4 bytes for wordLen + wordLen bytes for word + 4 bytes for count
        memcpy(message, &wordLen, sizeof(int));
        memcpy(message + sizeof(int), word, wordLen);
        memcpy(message + sizeof(int) + wordLen, &count, sizeof(int));
        write(STDOUT_FILENO, message, sizeof(int) + wordLen + sizeof(int));
    }

    close(STDIN_FILENO);

    // End timing
    t2 = (double) times(&tb2);
    real_time = (t2 - t1) / ticspersec;

    // Send real time to root
    // use fake length of word as a flag to indicate the time
    int timeFlag = 777;
    char message[sizeof(int) + sizeof(double)];
    memcpy(message, &timeFlag, sizeof(int));
    memcpy(message + sizeof(int), &real_time, sizeof(double));
    write(STDOUT_FILENO, message, sizeof(message));

    // Send signal USR2 to root
    kill(getppid(), SIGUSR2);
    return 0;
}
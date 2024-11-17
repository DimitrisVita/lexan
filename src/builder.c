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
    while (read(STDIN_FILENO, &c, 1) > 0) {
        // add word to map
        if (isalpha(c)) {
            word[wordIndex++] = tolower(c);
        } else if (wordIndex > 0) {
            word[wordIndex] = '\0';
            char *value = getMapValue(map, word);
            if (value == NULL) {
                addMapNode(map, strdup(word), strdup("1"));
            } else {
                int count = atoi(value);
                count++;
                char countStr[10];
                sprintf(countStr, "%d", count);
                deleteMapNode(map, word);
                addMapNode(map, strdup(word), strdup(countStr));
            }
            wordIndex = 0;
        }
    }

    for (MapNode node = getFirstMapNode(map); node != NULL; node = getNextMapNode(map, node)) {
        char *key = node->key;
        char *value = node->value;

        // Combine key and value as a string
        char result[256];
        snprintf(result, sizeof(result), "%s*%s\n", key, value);
        write(STDOUT_FILENO, result, strlen(result));
    }

    close(STDIN_FILENO);

    // End timing
    t2 = (double) times(&tb2);
    real_time = (t2 - t1) / ticspersec;

    // Send real time to root
    char timeLabel[256];
    sprintf(timeLabel, "TIME:%lf\n", real_time);
    write(STDOUT_FILENO, timeLabel, strlen(timeLabel));

    // Send signal USR2 to root
    kill(getppid(), SIGUSR2);
    return 0;
}
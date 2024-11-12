#include "common.h"
#include "ADTMap.h"
#include <time.h>

int main(int argc, char *argv[]) {

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
                addMapNode(map, word, "1");
            } else {
                int count = atoi(value);
                count++;
                char countStr[10];
                sprintf(countStr, "%d", count);
                addMapNode(map, word, countStr);
            }
            wordIndex = 0;
        }
    }



    close(STDIN_FILENO);

    // Send signal USR2 to root
    kill(getppid(), SIGUSR2);
    return 0;
}
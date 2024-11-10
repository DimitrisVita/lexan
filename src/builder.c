#include "common.h"
#include "ADTMap.h"
#include <time.h>

int main(int argc, char *argv[]) {
    clock_t start = clock();

    // // Create a dynamic structure to store words and their counts
    // // For simplicity, using a map-like structure (e.g., hash table)
    // Map wordCountMap = createMap(1000);

    // char word[128];
    // while (read(STDIN_FILENO, word, sizeof(word)) > 0) {
    //     incrementWordCount(wordCountMap, word);
    // }

    // // Send results back to the root process
    // sendResultsToRoot(wordCountMap);

    // clock_t end = clock();
    // double time_spent = (double)(end - start) / CLOCKS_PER_SEC;

    // // Send execution time to the root process
    // sendTimeToRoot(time_spent);

    // Αποστολή σήματος USR2 στον root
    kill(getppid(), SIGUSR2);

    return 0;
}
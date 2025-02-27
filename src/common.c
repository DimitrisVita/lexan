#include "common.h"

// Function to read a word from stdin
void freeWord(void *data) {
    Word *word = (Word *)data;
    free(word->word);
    free(word);
}

// Function to compare words
int compareWords(const void *a, const void *b) {
    Word *wordA = (Word *)a;
    Word *wordB = (Word *)b;

    return wordB->count - wordA->count;
}

ssize_t safeRead(int fd, void *buffer, size_t count) {
    ssize_t totalBytesRead = 0;
    ssize_t bytesRead;

    while (totalBytesRead < count) {
        bytesRead = read(fd, (char *)buffer + totalBytesRead, count - totalBytesRead);
        if (bytesRead == -1) {
            if (errno == EINTR) 
                continue;
            else
                return -1;
        } else if (bytesRead == 0)
            break;
        else
            totalBytesRead += bytesRead;
    }

    return totalBytesRead;
}
#include "common.h"

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

void sendTimeToRoot(double time_spent) {
    char timeLabel[256];
    sprintf(timeLabel, "TIME: %lf\n", time_spent);
    write(STDOUT_FILENO, timeLabel, strlen(timeLabel));
}
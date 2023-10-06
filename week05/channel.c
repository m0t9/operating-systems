#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define R 0
#define W 1

#define MAX_MSG_LEN 1024

int main() {
    int fd[2];
    if (pipe(fd) == -1) {
        return EXIT_FAILURE;
    }

    if (fork() == 0) {
        // Publisher
        close(fd[R]);
        for (;;) {
            printf("Input message for subscriber: ");
            char message[MAX_MSG_LEN];
            fgets(message, MAX_MSG_LEN, stdin);
            write(fd[W], message, strlen(message) + 1);
            sleep(1);
        }     
    } else {
        // Subscriber
        close(fd[W]);
        for (;;) {
            char buffer[MAX_MSG_LEN];
            read(fd[R], buffer, sizeof buffer);
            printf("Subscriber obtained message: %s", buffer);
        }
        
    }
    return EXIT_SUCCESS;
}

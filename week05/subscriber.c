#include <stdlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>

#define PIPE_NAME "/tmp/ex1/s%ld"
#define MAX_MSG_LEN 1024
#define MAX_CHILDREN 4
#define MAX_STR_LEN 100

int fd;

void signal_handler(int signum) {
    close(fd);
    exit(signum);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        return EXIT_FAILURE;
    }
    
    signal(SIGINT, &signal_handler);

    size_t pipe_idx = atoll(argv[1]);
    char pipe[MAX_STR_LEN];
    sprintf(pipe, PIPE_NAME, pipe_idx);
    fd = open(pipe, O_RDONLY);

    printf("Subscriber %ld is connected to channel\n", pipe_idx); 
    char message[MAX_MSG_LEN];
    for (;;) {
        int code = read(fd, message, sizeof(message));
        if (code == 0 || code == -1) {
            break;
        }
        printf("Obtained message:\n");
        printf("%s\n", message);
    }
    return 0; 
}

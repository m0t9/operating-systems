#include <stdlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>

#define PIPE_PATH "/tmp/ex1/"
#define PIPE_NAME "/tmp/ex1/s%ld"
#define MAX_MSG_LEN 1024
#define MAX_SUBSCRIBERS 4
#define MAX_STR_LEN 100
#define R 0
#define W 1

size_t n;
int fd[MAX_SUBSCRIBERS];
int child[MAX_SUBSCRIBERS][2];
pid_t forked[MAX_SUBSCRIBERS];

void setup_pipes() {
    mkdir(PIPE_PATH, 0777);
    for (size_t idx = 1; idx <= n; ++idx) {
        char pipe_data[MAX_STR_LEN];
        sprintf(pipe_data, PIPE_NAME, idx);
        mkfifo(pipe_data, 0777);
        fd[idx] = open(pipe_data, O_RDWR);
        pipe(child[idx]);
    }
}

void revoke_pipes() { 
    for (size_t idx = 1; idx <= n; ++idx) {
        char pipe[MAX_STR_LEN];
        sprintf(pipe, PIPE_NAME, idx);
        close(fd[idx]);
        unlink(pipe);
        remove(pipe);
    }
    rmdir(PIPE_PATH);
}

void signal_handler(int signum) {
    revoke_pipes();
    for (size_t idx = 1; idx <= n; ++idx) {
        kill(forked[idx], signum);
    }
    exit(signum);
}

void send_message(const char* message, size_t idx) {
    char pipe[MAX_STR_LEN];
    sprintf(pipe, PIPE_NAME, idx); 

    write(fd[idx], message, strlen(message) + 1);
}

void read_message(char* str) {
    printf("Input your message:\n");
    scanf("%[^\n]", str);
    getchar();
}

int main(int argc, char* argv[]) {
    revoke_pipes();
    signal(SIGINT, &signal_handler);
    if (argc != 2) {
        printf("fail");
        return EXIT_FAILURE;
    }   

    n = atoll(argv[1]);
    setup_pipes();

    for (size_t i = 1; i <= n; ++i) {
        if (!(forked[i] = fork())) {
            close(child[i][W]);
            for (;;) {
                char message[MAX_MSG_LEN];
                read(child[i][R], message, sizeof message);
                send_message(message, i);
            }
            return 0;
        }
    }

    for (size_t i = 1; i <= n; ++i) {
        close(child[i][R]);
    }
    
    for (;;) {
        char message[MAX_MSG_LEN];
        read_message(message);

        for (size_t i = 1; i <= n; ++i) {
            write(child[i][W], message, strlen(message) + 1);
        }
    }
    return 0;
}

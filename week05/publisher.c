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
#define MAX_CHILDREN 4
#define MAX_STR_LEN 100

size_t n;
int fd[MAX_CHILDREN];

void setup_pipes() {
    mkdir(PIPE_PATH, 0777);
    for (size_t idx = 1; idx <= n; ++idx) {
        char pipe[MAX_STR_LEN];
        sprintf(pipe, PIPE_NAME, idx);
        mkfifo(pipe, 0777);
        fd[idx] = open(pipe, O_RDWR);
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
    exit(signum);
}

void send_message(const char* message) {
    for (size_t idx = 1; idx <= n; ++idx) {
        char pipe[MAX_STR_LEN];
        sprintf(pipe, PIPE_NAME, idx); 

        write(fd[idx], message, strlen(message) + 1);
   } 
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

    for (;;) {
        char message[MAX_MSG_LEN];
        read_message(message);
        send_message(message);
    }
    return 0;
}

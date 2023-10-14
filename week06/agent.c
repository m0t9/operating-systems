#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <assert.h>

#define PID_PATH "agent.pid"
#define MAX_MSG_LEN 100
#define MSG_FILE_NAME "text.txt"

void write_pid() {
    FILE* pid_file = fopen(PID_PATH, "w");
    fprintf(pid_file, "%d", getpid());
    fclose(pid_file);
}

void print_message() {
    char message[MAX_MSG_LEN];
    FILE* msg_file = fopen(MSG_FILE_NAME, "r");
    fscanf(msg_file, "%[^\n]", message);
    fclose(msg_file);
    printf("%s\n", message);
}

void sigusr1_handler(int) {
    print_message();
}

void sigusr2_handler(int) {
    printf("Process terminating...\n");
    remove(PID_PATH);
    exit(SIGTERM);
}

int main() {
    signal(SIGUSR1, &sigusr1_handler);
    signal(SIGUSR2, &sigusr2_handler);
    write_pid();
    print_message();
    for (;;) {
        pause();
    }
}

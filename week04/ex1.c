#include <stdio.h>
#include <unistd.h>
#include <time.h>

void print_process_data() {
    printf("PID = %d, parent's PID = %d\n", getpid(), getppid());
}

void print_execution_time() {
    printf("Execution time of process with PID = %d is %lu clocks\n",
            getpid(), clock());
}

int main() {
    if (fork()) {
        fork();
    }
    print_process_data();
    print_execution_time();
}

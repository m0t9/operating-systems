#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>

#define MS 1000

void print_process_data() {
    printf("PID = %d, parent's PID = %d\n", getpid(), getppid());
}

void print_execution_time(clock_t start) {
    printf("Execution time of process with PID = %d is %f milliseconds\n",
            getpid(), (float) ((clock() - start) * MS) / CLOCKS_PER_SEC);
}

int main() {
    clock_t timer = clock();
    pid_t child = fork();
    if (child == 0) {
        timer = clock();
    } else {
        child = fork();
        if (child == 0) {
            timer = clock();
        }
    }
    while (wait(NULL) > 0);
    print_process_data();
    print_execution_time(timer);
    return 0;
}

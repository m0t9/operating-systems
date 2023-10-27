#include <sys/time.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include <stdio.h>

#define MEMORY_SIZE 10485760
#define TIME 10

void print_rusage(size_t idx) {
    printf("Iteration %ld\n", idx);
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == -1) {
        printf("Error getting resources usage\n");
        return;
    }
    printf("ru_maxrss = %ld\n", usage.ru_maxrss);
    printf("ru_ixrss = %ld\n", usage.ru_ixrss);
    printf("ru_idrss = %ld\n", usage.ru_idrss);
    printf("ru_isrss = %ld\n", usage.ru_isrss);
}

int main() {
    for (size_t idx = 0; idx < TIME; ++idx) {
        void* block = malloc(MEMORY_SIZE);
        memset(block, 0, MEMORY_SIZE);
        print_rusage(idx);
        sleep(1);
    }
}

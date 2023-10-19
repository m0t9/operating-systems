#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#define SIZE 10000000
#define INPUT "queries.txt"
#define CMD_LEN 10

typedef unsigned int uint;

uint memory[SIZE];

void reset() {
    for (int idx = 0; idx < SIZE; ++idx) {
        memory[idx] = 0;
    }
}

void clear(uint adrs)  {
    for (int idx = 0; idx < SIZE; ++idx) {
        if (memory[idx] == adrs) {
            memory[idx] = 0;
        }
    }
}

void write_to_memory(int begin, int size, uint adrs)  {
    for (int idx = begin; idx < begin + size; ++idx) {
        memory[idx] = adrs;
    }
}

void allocate_first_fit(uint adrs, int size) {
    int left = 0;
    int right = -1;
    while (right - left < size) {
        left = right + 1;
        while (memory[left] != 0) {
            ++left;
        }
        right = left + 1;
        while (right < SIZE && memory[right] == 0) {
            ++right;
        }
    }
    write_to_memory(left, size, adrs);
}


void allocate_best_fit(uint adrs, int size) {
    int left = 0;
    int right = -1;

    int best_left = 0;
    int best_right = 2 * SIZE;
    while (right < SIZE) {
        left = right + 1;
        while (memory[left] != 0) {
            ++left;
        }
        right = left + 1;
        while (right < SIZE && memory[right] == 0) {
            ++right;
        }
        if (right - left >= size 
                && right - left < best_right - best_left) {
            best_left = left;
            best_right = right;
        }
    }
    write_to_memory(best_left, size, adrs);
}

void allocate_worst_fit(uint adrs, int size) {
    int left = 0;
    int right = -1;

    int best_left = 0;
    int best_right = 0;
    while (right < SIZE) {
        left = right + 1;
        while (memory[left] != 0) {
            ++left;
        }
        right = left + 1;
        while (right < SIZE && memory[right] == 0) {
            ++right;
        }
        if (right - left >= size 
                && right - left > best_right - best_left) {
            best_left = left;
            best_right = right;
        }
    }
    write_to_memory(best_left, size, adrs);
}

int operations_cnt = 0;
void allocator_test(void (*alloc) (uint, int)) { 
    FILE* queries = fopen(INPUT, "r");
    if (queries == NULL) {
        printf("No file found\n");
        exit(-1);
    }
    
    char cmd[CMD_LEN];
    fscanf(queries, "%s", cmd); 
    while (strcmp(cmd, "end")) {
        ++operations_cnt;
        if (!strcmp("clear", cmd)) {
            uint mark;
            fscanf(queries, "%u", &mark);
            clear(mark);
        } else if (!strcmp("allocate", cmd)) {
            uint adrs;
            int size;
            fscanf(queries, "%u", &adrs);
            fscanf(queries, "%d", &size);
            alloc(adrs, size);
        } else {
            exit(-1);
        }
        fscanf(queries, "%s", cmd); 
    }
}

void run_test(void (*alloc) (uint, int), const char* algo_name) {
    clock_t start, end;
    start = clock();
    allocator_test(alloc);
    end = clock();
    printf("Throughput for %s allocation algorithm is %f ops/sec\n", 
            algo_name,
            (float) operations_cnt / ((float)(end - start) / ((float)CLOCKS_PER_SEC)));
    operations_cnt = 0;
}

int main() {
   printf("Testing results are:\n");
   run_test(&allocate_first_fit, "First-Fit");
   run_test(&allocate_best_fit, "Best-Fit");
   run_test(&allocate_worst_fit, "Worst-Fit");
}

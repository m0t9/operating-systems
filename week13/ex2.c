#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define INPUT_FILE "input.txt"

#define MAX_PROCESSES 100
#define MAX_RESOURCE_TYPES 100
#define MAX_LINE_LENGTH 100

#define NO_DEADLOCK_MSG "No deadlock detected in system.\n"
#define DEADLOCK_MSG "Warning! Deadlock detected.\n"
#define DEADLOCKED_PROC_MSG "Next processes will be deadlocked (0-indexation): "

size_t completed_processes = 0;
int currently_allocated[MAX_PROCESSES][MAX_RESOURCE_TYPES];
int available[MAX_RESOURCE_TYPES];
int requests[MAX_PROCESSES][MAX_RESOURCE_TYPES];
int all_resources[MAX_RESOURCE_TYPES];
bool safe[MAX_PROCESSES];

size_t resources, processes;
FILE* inf;

bool can_be_fulfilled(int proc_idx) {
    bool enough_resources = true;
    for (size_t res_idx = 0; res_idx < resources; res_idx++) {
        if (requests[proc_idx][res_idx] > available[res_idx]) {
            enough_resources = false;
            break;
        }
    }
    return enough_resources;
}

void release_resources(int proc_idx) {
    safe[proc_idx] = true;
    for (size_t res_idx = 0; res_idx < resources; res_idx++) {
        available[res_idx] += currently_allocated[proc_idx][res_idx];
    }
}

bool try_distribute_resources() {
    bool success = false;
    for (size_t proc_idx = 0; proc_idx < processes; proc_idx++) {
        if (safe[proc_idx]) {
            continue;
        }
        if (can_be_fulfilled(proc_idx)) {
            release_resources(proc_idx);
            success = true;
            completed_processes++;
        }
    }
    return success;
}

void bankers_algorithm() {
    while (try_distribute_resources() && completed_processes < processes);
    if (completed_processes == processes) {
        printf(NO_DEADLOCK_MSG);
        return;
    }
    printf(DEADLOCK_MSG);
    printf(DEADLOCKED_PROC_MSG);
    for (size_t proc_idx = 0; proc_idx < processes; proc_idx++) {
        if (!safe[proc_idx]) {
            printf("%ld ", proc_idx);
        }
    }
    printf("\n");
}

void parse_limits() {
    fseek(inf, 0, SEEK_SET);

    char line[MAX_LINE_LENGTH];
    
    // Read first line and extract the number of resources
    fgets(line, MAX_LINE_LENGTH, inf);
    size_t whitespaces_count = 0;
    for (size_t idx = 0; line[idx] != '\0'; idx++) {
        whitespaces_count += (line[idx] == ' ');
    }
    resources = whitespaces_count + 1;

    size_t lines_cnt = 0;
    while (fgets(line, MAX_LINE_LENGTH, inf) != NULL) {
        // count only non-empty strings
        lines_cnt += (line[0] != '\n');
    }
    processes = (lines_cnt - 1) / 2;

    fseek(inf, 0, SEEK_SET);
}

int main() {
    inf = fopen(INPUT_FILE, "r");
    parse_limits();
    
    // Read maximum available resources cnt
    for (size_t idx = 0; idx < resources; idx++) {
        fscanf(inf, "%d", &all_resources[idx]);
    }

    // Read currently available resources 
    for (size_t idx = 0; idx < resources; idx++) {
        fscanf(inf, "%d", &available[idx]);
    }

    // Read allocation matrix
    for (size_t proc_idx = 0; proc_idx < processes; proc_idx++) {
        for (size_t res_idx = 0; res_idx < resources; res_idx++) {
            fscanf(inf, "%d", &currently_allocated[proc_idx][res_idx]);
        }
    }

    // Read requests matrix
    for (size_t proc_idx = 0; proc_idx < processes; proc_idx++) {
        for (size_t res_idx = 0; res_idx < resources; res_idx++) {
            fscanf(inf, "%d", &requests[proc_idx][res_idx]);
        }
    }

    bankers_algorithm();
}

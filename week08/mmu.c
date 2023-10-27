#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <signal.h>
#include <memory.h>

#define PAGETABLE "/tmp/ex2/pagetable"

#define PT_OPEN_ERROR "Pagetable open error\n"
#define PT_TRUNCATE_ERROR "Pagetable truncation error\n"
#define MMAP_ERROR "Mmap error\n"
#define PT_REMOVE_ERROR "Pagetable file was not removed\n"
#define NO_ARGS_ERROR "Not enough args\n"
#define ARG_PARSE_ERROR "Error while argument parsing\n"

typedef struct {
    bool valid;
    int frame;
    bool dirty;
    int referenced;
} PTE;

#define raise_error(msg) {   \
    fprintf(stderr, msg);    \
    exit(EXIT_FAILURE);      \
}

PTE* pagetable;
int pagetable_fd;
size_t p;
size_t pagetable_size;
int pager_pid;

void open_pagetable() {
    pagetable_fd = open(PAGETABLE, O_RDWR);
    if (pagetable_fd == -1) {
        raise_error(PT_OPEN_ERROR);
    }
    pagetable = (PTE*) mmap(NULL, pagetable_size, PROT_READ | PROT_WRITE, MAP_SHARED, pagetable_fd, 0);
    if (pagetable == NULL) {
        raise_error(MMAP_ERROR);
    }
}

bool is_page_in_ram(size_t page_idx) {
    return pagetable[page_idx].valid;
}

void mmu_termination() {
    printf("Done all requests.\nMMU sends SIGUSR1 to the pager.\n");
    munmap((void*) pagetable, pagetable_size);
    close(pagetable_fd);
    kill(pager_pid, SIGUSR1);
    printf("MMU terminates.\n");
}

void handler(int) {

}

void print_sep() {
    printf("--------------------\n");
}

void print_pagetable() {
    printf("Page table\n");
    for (size_t idx = 0; idx < p; ++idx) {
        printf("Page %ld ---> valid=%d, frame=%d, dirty=%d, referenced=%d\n",
                idx, pagetable[idx].valid, pagetable[idx].frame,
                pagetable[idx].dirty, pagetable[idx].referenced);
    }
    print_sep();
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        raise_error(NO_ARGS_ERROR);
    }

    p = atoll(argv[1]);

    pagetable_size = sizeof(PTE) * p;
    open_pagetable();
    printf("Initialized page table\n");
    print_pagetable();
    
    pager_pid = atoi(argv[argc - 1]);
    signal(SIGCONT, handler);

    for (size_t idx = 2; idx < (size_t) argc - 1; ++idx) {
        char type;
        size_t page_idx;
        if (sscanf(argv[idx], "%c%ld", &type, &page_idx) != 2) {
            raise_error(ARG_PARSE_ERROR);
        }

        char* request_type = (type == 'W' ? "Write" : "Read");
        printf("%s Request for page %ld\n", request_type, page_idx);
        if (is_page_in_ram(page_idx)) {
            printf("It is a valid page\n");
        } else {
            printf("It is not a valid page ---> page fault\n");
            pagetable[page_idx].referenced = getpid();
            printf("Ask pager to load it from disk (SIGUSR1 signal) and wait\n");
            kill(pager_pid, SIGUSR1);
            pause();
            printf("MMU resumed by SIGCONT signal from pager\n");
        }
        if (type == 'W') {
            pagetable[page_idx].dirty = true;
        }
        print_pagetable();
    }

    mmu_termination();
}

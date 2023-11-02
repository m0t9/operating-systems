#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <signal.h>
#include <memory.h>

#define DATA_LEN 8
#define MAX_SIZE 2000
#define RANDOM "/dev/random"
#define WORK_DIR "/tmp/ex2/"
#define PAGETABLE "/tmp/ex2/pagetable"
#define MAJOR_BIT 7

#define PT_CREATE_ERROR "Pagetable creation error\n"
#define PT_TRUNCATE_ERROR "Pagetable truncation error\n"
#define MMAP_ERROR "Mmap error\n"
#define PT_REMOVE_ERROR "Pagetable file was not removed\n"
#define NO_ARGS_ERROR "Not enough args\n"

typedef struct {
    bool valid;
    int frame;
    bool dirty;
    int referenced;
    int nfu_counter;
    unsigned char aging_counter;
} PTE;

char RAM[MAX_SIZE][DATA_LEN];
char disk[MAX_SIZE][DATA_LEN];

size_t p;
size_t f;

size_t pagetable_size;
int pagetable_fd;
PTE* pagetable;
size_t disk_accesses;

#define raise_error(msg) {   \
    fprintf(stderr, msg);    \
    exit(EXIT_FAILURE);      \
}

void initialize_disk() {
    int random = open(RANDOM, O_RDONLY);
    for (size_t idx = 0; idx < p; ++idx) {
        for (size_t idx2 = 0; idx2 < DATA_LEN - 1; ++idx2) {
            char sym;
            do {
                read(random, &sym, sizeof(sym));
            } while (!isprint(sym));
            disk[idx][idx2] = sym;
        }
        disk[idx][DATA_LEN - 1] = '\0';
    }
    close(random);
}

void initialize_pagetable() {
    mkdir(WORK_DIR, S_IRWXU);
    pagetable_size = p * sizeof(PTE);
    pagetable_fd = open(PAGETABLE, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (pagetable_fd == -1) {
        raise_error(PT_CREATE_ERROR);
    }
    if (ftruncate(pagetable_fd, pagetable_size) == -1) {
        raise_error(PT_TRUNCATE_ERROR);
    }
    pagetable = (PTE*) mmap(NULL, pagetable_size, 
            PROT_READ | PROT_WRITE, MAP_SHARED, pagetable_fd, 0);
    if (pagetable == NULL) {
        raise_error(MMAP_ERROR);    
    }

    for (size_t idx = 0; idx < p; ++idx) {
        pagetable[idx].valid = false;
        pagetable[idx].dirty = false;
        pagetable[idx].frame = -1;
        pagetable[idx].referenced = 0;
    }
}

size_t find_free_frame() {
    size_t free_frame = 0;
    for (; free_frame < f; ++free_frame) {
        bool ok = true;
        for (size_t idx = 0; idx < DATA_LEN; ++idx) {
            if (RAM[free_frame][idx] != 0) {
                ok = false;
                break;
            }
        }
        if (ok) {
            break;
        }
    }
    return free_frame;
}

void swap_page_in(size_t page_idx, size_t frame_idx) {
    printf("Copy data from the disk (page=%ld) to RAM (frame=%ld)\n", 
            page_idx, frame_idx);
    memcpy(RAM[frame_idx], disk[page_idx], DATA_LEN);
    ++disk_accesses;
}

size_t random_page() {
    size_t pick = rand() % f;
    for (size_t idx = 0; idx < p; ++idx) {
        if (!pagetable[idx].valid) {
            continue;
        }
        if (pick == 0) {
            return idx;
        }
        --pick;
    }
    exit(EXIT_FAILURE);
} 

size_t nfu() {
    int min_count = INT_MAX;
    size_t idx_to_evict = 0;
    for (size_t idx = 0; idx < p; ++idx) {
        if (!pagetable[idx].valid) {
            continue;
        }
        if (pagetable[idx].nfu_counter < min_count) {
            min_count = pagetable[idx].nfu_counter;
            idx_to_evict = idx;
        }
    }
    return idx_to_evict;
} 

unsigned char popcount(unsigned char x) {
    unsigned char cnt = 0;
    for (int bit = 0; bit <= MAJOR_BIT; ++bit) {
        cnt += (((1 << bit) & (x)) != 0);
    }
    return cnt;
}

size_t aging() {
   unsigned char min_popcount = UCHAR_MAX;
   size_t idx_to_evict = 0;
   for (size_t idx = 0; idx < p; ++idx) {
       if (!pagetable[idx].valid) {
           continue;
       }
       if (popcount(pagetable[idx].aging_counter) < min_popcount) {
           min_popcount = popcount(pagetable[idx].aging_counter);
           idx_to_evict = idx;
       }
   }
   return idx_to_evict;
} 

size_t (*page_replacement) ();

int choose_victim_page() {
    int idx = page_replacement();
    printf("Choose a victim page %d\n", idx);
    return idx;
}

void swap_page_out(size_t page_idx) { 
    printf("Victim page %ld is dirty, swapping to disk\n", page_idx);
    memcpy(disk[page_idx], RAM[pagetable[page_idx].frame], DATA_LEN);
    ++disk_accesses;
}

void print_sep() {
    printf("--------------------\n");
}

void print_ram() {
    printf("RAM array\n");
    for (size_t idx = 0; idx < f; ++idx) {
        printf("Frame %ld ---> %s\n", idx, RAM[idx]);
    }
    print_sep();
}

void print_disk() {
    printf("Disk array\n");
    for (size_t idx = 0; idx < p; ++idx) {
        printf("Page %ld ----> %s\n", idx, disk[idx]);
    }
    print_sep();
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

void pager_termination(int code) {
    printf("%ld disk accesses in total\n", disk_accesses);
    printf("Pager is terminated\n");
    munmap((void*) pagetable, pagetable_size);
    close(pagetable_fd);
    if (remove(PAGETABLE) != 0) {
        raise_error(PT_REMOVE_ERROR);
    }
    exit(code);
}


void refer(size_t page_idx) {
    pagetable[page_idx].aging_counter |= (1 << MAJOR_BIT);
    ++pagetable[page_idx].nfu_counter;
}

void find_referenced(int code) {
    bool found = false;
    int mmu_pid = -1;
    for (size_t page_idx = 0; page_idx < p; ++page_idx) {
        if (!pagetable[page_idx].referenced) {
            continue;
        }
        refer(page_idx);

        mmu_pid = pagetable[page_idx].referenced;
        printf("A disk access request from MMU Process (pid=%d)\n", mmu_pid);
        
        printf("Page %ld is referenced\n", page_idx);
        size_t frame_idx = find_free_frame();
        
        if (frame_idx < f) {
            printf("We can allocate it to free frame %ld\n", frame_idx);
        } else {
            printf("We do not have free frames in RAM\n");
            size_t victim = choose_victim_page();
            printf("Replace/Evict it with page %ld to be allocated to frame %d\n",
                    page_idx, pagetable[victim].frame);
            if (pagetable[victim].dirty) {
                swap_page_out(victim);
                pagetable[victim].dirty = false;
            }
            frame_idx = pagetable[victim].frame;
            pagetable[victim].valid = false;
            pagetable[victim].frame = -1;
            pagetable[victim].referenced = 0;
        }
        
        pagetable[page_idx].valid = true;
        pagetable[page_idx].dirty = false;
        pagetable[page_idx].referenced = 0;
        pagetable[page_idx].frame = frame_idx;
        swap_page_in(page_idx, frame_idx);
        print_ram();
        found = true;
        break;
    }
    if (found) {
        printf("%ld disk accesses so far\n", disk_accesses);
        printf("Resuming MMU process\n");
        print_sep();
        usleep(5);
        kill(mmu_pid, SIGCONT);
    } else {
        pager_termination(code);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        raise_error(NO_ARGS_ERROR);
    }

    p = atoll(argv[1]);
    f = atoll(argv[2]);

    if (!strcmp(argv[3], "random")) {
        page_replacement = random_page;
        printf("Random page replacement algorithm was selected\n");
    } else if (!strcmp(argv[3], "nfu")) {
        page_replacement = nfu;
        printf("NFU page replacement algorithm was selected\n");
    } else if (!strcmp(argv[3], "aging")) {
        page_replacement = aging;
        printf("Aging page replacement algorithm was selected\n");
    } else {
        printf("Invalid name for page replacement algorithm!");
        exit(EXIT_FAILURE);
    }

    initialize_disk();
    initialize_pagetable();

    printf("Initialized pagetable\n");
    print_pagetable();
    printf("Initialized RAM\n");
    print_ram();
    printf("Initialized disk\n");
    print_disk();

    signal(SIGUSR1, find_referenced);

    for (;;) {
        pause();
    }
}

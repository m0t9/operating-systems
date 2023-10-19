#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

#define TEXT "text.txt"
#define STR_LEN 1023
#define STR_COUNT (500 * 1024)
#define FILE_LEN (STR_LEN * STR_COUNT)
#define RANDOM "/dev/random"

void generate_text() {
    FILE* file = fopen(TEXT, "w"); 
    int random = open(RANDOM, O_RDONLY);
    for (size_t idx = 0; idx < STR_COUNT; ++idx) {
        for (size_t jdx = 0; jdx < STR_LEN; ++jdx) {
            char sym;
            do {
                read(random, &sym, sizeof(sym));
            } while (!isprint(sym));
            fprintf(file, "%c", sym);
        }
        fprintf(file, "\n");
    }
    close(random);
    fclose(file);
}

void count_and_rewrite_text() { 
    off_t chunk_size = sysconf(_SC_PAGESIZE);
    
    int file = open(TEXT, O_RDWR);

    off_t begin = 0;
    size_t uppercase_count = 0;

    while (begin != FILE_LEN) { 
        void* chunk = mmap(NULL, chunk_size, PROT_WRITE, MAP_SHARED, file, begin);
        char* casted_chunk = (char*) chunk;
        for (int idx = 0; idx < chunk_size; ++idx) {
            if (isupper(casted_chunk[idx])) {
                ++uppercase_count;
                casted_chunk[idx] = tolower(casted_chunk[idx]);
            }
        }
        munmap(chunk, chunk_size);
        begin += chunk_size;
    }
    printf("Number of uppercase letters is %lu\n", uppercase_count);
    close(begin);
}
int main() {
    generate_text();
    count_and_rewrite_text();
}

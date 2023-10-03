#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

#define VECTOR_SIZE 120
#define MAX_ELEM 100
#define BUFFER "temp.txt"

typedef unsigned long int size_t;

void dot_product_part(int* a, int* b, size_t begin, size_t end) {
    int part = 0;
    for (size_t idx = begin; idx < end; ++idx) {
        part += a[idx] * b[idx];
    }
    FILE* buffer = fopen(BUFFER, "a");
    fprintf(buffer, "%d\n", part);
    fclose(buffer);
}

void clear_buffer() {
    fclose(fopen(BUFFER, "w"));
}

int main() {
    clear_buffer();

    int a[VECTOR_SIZE];
    int b[VECTOR_SIZE];
    for (size_t idx = 0; idx < VECTOR_SIZE; ++idx) {
        a[idx] = rand() % MAX_ELEM;
        b[idx] = rand() % MAX_ELEM;
    }

    size_t proc_c;
    printf("How many processes to launch: ");
    scanf("%ld", &proc_c);
    size_t seg = VECTOR_SIZE / proc_c;

    size_t begin;
    size_t end;

    for (size_t p_idx = 0; p_idx < proc_c; ++p_idx) {
        if (fork() == 0) {
           begin = p_idx * seg;
           end = begin + seg;
           dot_product_part(a, b, begin, end);
           exit(EXIT_SUCCESS);
        }
    }

    while (wait(NULL) > 0);

    FILE* buffer = fopen(BUFFER, "r");
    int part = 0;
    int dot_product = 0;
    while (fscanf(buffer, "%d", &part) != EOF) {
        dot_product += part;
    }
    fclose(buffer);

    printf("Dot product is %d\n", dot_product);
}


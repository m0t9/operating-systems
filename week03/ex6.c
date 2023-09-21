#include <stdio.h>

typedef unsigned long int size_t;

void swap(int* a, int* b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

int* partition(int* begin, int* end) {
    int* pivot = begin;
    for (int* ptr = begin + 1; ptr != end; ++ptr) {
        if (*ptr < *pivot) {
            swap(pivot, ptr);
            swap(pivot + 1, ptr);
            ++pivot;
        }
    }
    return pivot;
}

void quicksort(int* begin, int* end) {
    if (end - begin <= 0) {
        return;
    }
    int* pivot = partition(begin, end);
    quicksort(begin, pivot);
    quicksort(pivot + 1, end);
}

int main() {
    size_t n;
    printf("Input the length of the array: ");
    scanf("%ld", &n);
    printf("Input %ld integers (array elements): ", n);

    int a[n];
    for (size_t idx = 0; idx < n; scanf("%d", &a[idx]), ++idx);
    quicksort(a, a + n);

    printf("Sorted array is:\n");
    for (size_t idx = 0; idx < n; printf("%d ", a[idx]), ++idx);
    printf("\n");
}

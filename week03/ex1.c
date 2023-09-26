#include <stdlib.h>
#include <stdio.h>

int const_tri(int* const p, int n) {
    *p = *(p + 1) = 0;
    *(p + 2) = 1;
    while (n > 1) {
        *(p + 2) += *p + *(p + 1);
        *(p + 1) = *(p + 2) - *p - *(p + 1);
        *p = *(p + 2) - *p - *(p + 1);
        --n;
    }
    return *(p + 2);
}

int main() {
    const int x = 1;
    int const *q = &x;
    int* const p = (int*)malloc(sizeof(int) * 3);
    
    *p = x;
    *(p + 1) = x;
    *(p + 2) = 2 * x;
    printf("The address of 1st cell is %p\n", p);
    printf("The address of 2nd cell is %p\n", p + 1);
    printf("The address of 3rd cell is %p\n", p + 2);

    printf("Input index n ( >= 1) to get n-th Tribonacci number: ");
    int n;
    scanf("%d", &n);
    printf("%d-th Tribonacci number is %d\n", n, const_tri(p, n));
    free(p);
    return 0;
}

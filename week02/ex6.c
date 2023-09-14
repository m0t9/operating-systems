#include <stdio.h>

void print_right_triangle(int n) {
    for (int idx = 0; idx < n; ++idx) {
        for (int col = 0; col < idx + 1; ++col) {
            printf("*");
        }
        printf("\n");
    }
}

void print_square(int n) {
    for (int idx = 0; idx < n; ++idx) {
        for (int col = 0; col < n; ++col) {
            printf("*");
        }
        printf("\n");
    }
}

void print_triangle(int n) {
    for (int idx = 1; idx <= n / 2; ++idx) {
        for (int col = 0; col < idx; ++col) {
            printf("*");
        }
        printf("\n");
    }
    for (int idx = n / 2 + n % 2; idx > 0; --idx) {
        for (int col = 0; col < idx; ++col) {
            printf("*");
        }
        printf("\n");
    }
}

int main() {
    int n;
    scanf("%d", &n);
    printf("First picture:\n");
    print_right_triangle(n);
    printf("Second picture:\n");
    print_triangle(n);
    printf("Third picture:\n");
    print_square(n);
}

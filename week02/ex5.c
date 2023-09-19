#include <stdio.h>

int tribonacci(short n) {
    int i1 = 1, i2 = 0, i3 = 0;
    for (short idx = 0; idx < n; ++idx) {
        int i4 = i1 + i2 + i3;
        i1 = i2;
        i2 = i3;
        i3 = i4;
    }
    return i3;
}

int main() {
	printf("N = 4, Tribonacci(4) = %d\n", tribonacci(4));
	printf("N = 36, Tribonacci(36) = %d\n", tribonacci(36));
}

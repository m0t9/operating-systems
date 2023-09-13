#include <stdio.h>

#define MAX_LENGTH 256

typedef unsigned int uint;

char to_lowercase(char c) {
    if ('A' <= c && c <= 'Z') {
        return c + 32;
    }
    return c;
}

uint count(char s[], char c) {
    uint cnt = 0;
    for (uint idx = 0; s[idx] != '\0'; ++idx) {
        cnt += (to_lowercase(c) == to_lowercase(s[idx]) ? 1 : 0);
    }
    return cnt;
}

void countAll(char s[]) {
    for (uint idx = 0, last_cnt = 0; s[idx] != '\0'; ++idx) {
        uint cnt = count(s, s[idx]);
        if (last_cnt != 0) {
            printf(", ");
        }
        printf("%c:%u", to_lowercase(s[idx]), cnt);
        last_cnt = cnt;
    }
}

int main() {
    char s[MAX_LENGTH + 1];
    scanf("%256s", s);
    countAll(s);
}

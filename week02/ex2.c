#include <stdio.h>

#define MAX_LENGTH 256

int main() {
	char s[MAX_LENGTH];
	long int len = 0;
	while (len < MAX_LENGTH && (s[len] = getchar()) != '.' && s[len] != '\n') {	
		++len;
	}
	printf("\"");
	for (--len; len >= 0; --len) {
		putchar(s[len]);
	}
	printf("\"");
}

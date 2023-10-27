#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#define PATH "/tmp/ex1.pid"
#define PASSWORD_LEN 8
#define RANDOM "/dev/random"

void write_pid() {
    FILE* pid_file = fopen(PATH, "w");
    fprintf(pid_file, "%d", getpid());
    fclose(pid_file);
}

void write_password() {
    char* password = (char*) malloc(5 + PASSWORD_LEN + 1);
    strcpy(password, "pass:");
    int random = open(RANDOM, O_RDONLY);
    for (size_t idx = 0; idx < PASSWORD_LEN; ++idx) {
        char sym;
        do {
            read(random, &sym, 1);
        } while (!isprint(sym));
        password[5 + idx] = sym;
    }
    close(random);
    password[PASSWORD_LEN + 6] = '\0';
}

int main() {
    write_pid();
    write_password();
    for (;;) {
        pause();
    }
}

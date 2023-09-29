#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_CMD_LENGTH 100
#define EXIT_CMD "exit"
#define MAX_ARG_COUNT 20
#define SEP " "
#define CD_CMD "cd"

#define bool int
#define true 1
#define false 0

typedef unsigned long int size_t;

void read_line(char str[]) {
    printf("$> ");
    scanf("%[^\n]", str);
    getchar();
}

int main() {
    char cmd[MAX_CMD_LENGTH];
    for (read_line(cmd); strcmp(cmd, EXIT_CMD); read_line(cmd)) {
        char* args[MAX_ARG_COUNT + 1];
        
        args[0] = strtok(cmd, SEP);
        
        size_t arg_cnt = 1;
        while ((args[arg_cnt] = strtok(NULL, SEP)) != NULL) {
            ++arg_cnt;
        }
        args[arg_cnt] = NULL;
        
        bool is_background = false;
        if (!strcmp(args[arg_cnt - 1], "&")) {
            is_background = true;
            args[--arg_cnt] = NULL;
        }

        pid_t child;
        if (!strcmp(args[0], CD_CMD) && arg_cnt == 2) {
            chdir(args[1]);
        } else if (!(child = fork())) {
            execvp(args[0], args);
            exit(EXIT_SUCCESS);
        } else {
            if (!is_background) {
                waitpid(child, NULL, 0); 
            }
        }
    }
    return 0;
}


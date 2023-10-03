#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

typedef unsigned int uint;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        return 0;
    } 
    
    uint n = atoi(argv[1]);
    for (uint idx = 0; idx < n; ++idx) {
        fork();
        sleep(5);
    }

    while (wait(NULL) > 0);
    exit(0);
}

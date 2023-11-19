#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

#define raise_error(ERROR) do { \
    printf(ERROR);              \
    exit(-1); } while (0) 

#define ROOT_OPEN_ERROR "Root directory open error\n"

int main() {
    DIR* root;
    if ((root = opendir("/")) == NULL) {
        raise_error(ROOT_OPEN_ERROR);
    }
    for (struct dirent* entry = readdir(root); entry != NULL; entry = readdir(root)) {
        if (entry->d_name[0] == '.') {
            continue;
        }
        printf("%s\n", entry->d_name);
    }
    return 0;
}

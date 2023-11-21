#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/mman.h>
#include <string.h>

#define NO_ARGS_ERROR "No arguments error\n"
#define NO_FILE "No file with this name\n"
#define INVALID_BLOCK_READ "Invalid block num to read from file\n"
#define INVALID_BLOCK_WRITE "Invalid block num to write to file\n"
#define TOO_MANY_BLOCKS "Number of blocks for file is too big\n"
#define NOT_ENOUGH_SPACE "Not enough space on disk\n"
#define NO_FREE_INODES "No free inodes (max files count created)\n"

#define raise_error(ERROR) do {              \
    printf(ERROR);                           \
    exit(-1); } while (0) 
#define silent_error(ERROR) do {             \
    printf(ERROR);                           \
   } while (0) 

#define DISK_NAME_LEN 100
#define MAX_FILENAME_LEN 16
#define BLOCK_SIZE 1024
#define POINTERS_CNT 8
#define FREE_BLOCK_LIST_SIZE 128
#define INODES_CNT 16

char DISK_NAME[DISK_NAME_LEN];
FILE* disk;

typedef struct {
    char name[MAX_FILENAME_LEN];
    int size; 
    int block_pointers[POINTERS_CNT];
    int used; 
} inode;

typedef struct {
    char free_block_list[FREE_BLOCK_LIST_SIZE];
    inode inodes[INODES_CNT];
} super_block;

typedef struct {
    char content[BLOCK_SIZE];
} block;

void read_block_data(int block_num, block* block_ptr) {
    fseek(disk, BLOCK_SIZE * block_num, SEEK_SET);
    fread(block_ptr, sizeof(block), 1, disk);
}

void read_super_block(super_block* block_ptr) {
    read_block_data(0, (block*) block_ptr);
}

void write_block_data(int block_num, block* block_ptr) {
    fseek(disk, BLOCK_SIZE * block_num, SEEK_SET);
    fwrite((char*) block_ptr, sizeof(block), 1, disk);
}

void write_super_block(super_block* block_ptr) {
    write_block_data(0, (block*) block_ptr);
}

int create(char name[MAX_FILENAME_LEN], int size) {
    if (size > POINTERS_CNT) {
        silent_error(TOO_MANY_BLOCKS);
        return -1;
    }
    int pointers[POINTERS_CNT];
    super_block sb;
    read_super_block(&sb);
    int free_blocks = 0;
    for (size_t idx = 0; idx < FREE_BLOCK_LIST_SIZE; ++idx) {
        if (sb.free_block_list[idx] == 0) {
            pointers[free_blocks] = idx;
            ++free_blocks;
        } 
        if (free_blocks == size) {
            break;
        }
    }
    if (free_blocks < size) {
        silent_error(NOT_ENOUGH_SPACE);
        return -1;
    }
    for (inode_idx = 0; inode_idx < INODES_CNT; ++inode_idx) {
        if (!sb.inodes[inode_idx].used) {
            break;
        }
    }
    if (inode_idx == INODES_CNT) {
        silent_error(NO_FREE_INODES);
        return -1;
    }
    sb.inodes[inode_idx].size = size;
    sb.inodes[inode_idx].used = 1;
    strcpy(sb.inodes[inode_idx].name, name);
    for (int idx = 0; idx < size; ++idx) {
        sb.free_block_list[pointers[idx]] = '1';
        sb.inodes[inode_idx].block_pointers[idx] = pointers[idx];
    }
    write_super_block(&sb);
    return 0;
}
 
int delete(char name[MAX_FILENAME_LEN]) {
    super_block sb;
    read_super_block(&sb);
    int inode_idx;
    for (inode_idx = 0; inode_idx < INODES_CNT; ++inode_idx) {
        if (!sb.inodes[inode_idx].used) {
            continue;
        }
        if (!strcmp(sb.inodes[inode_idx].name, name)) {
            break;
        }
    }

    if (inode_idx == INODES_CNT) {
        silent_error(NO_FILE);
        return -1;
    }

    for (int idx = 0; idx < sb.inodes[inode_idx].size; ++idx) {
        int block_to_clear = sb.inodes[inode_idx].block_pointers[idx];
        sb.free_block_list[block_to_clear] = 0;
    }
    sb.inodes[inode_idx].used = 0;
    write_super_block(&sb);
    return 0;
}


void ls(void) { 
    super_block sb;
    read_super_block(&sb);
    printf("- ls\n");
    for (int idx = 0; idx < INODES_CNT; ++idx) {
        if (sb.inodes[idx].used) {
            printf("Filename: %s, filesize: %d\n", sb.inodes[idx].name, sb.inodes[idx].size);
        }
    }
} 

int read_block(char name[MAX_FILENAME_LEN], int block_num, char buf[BLOCK_SIZE]) {
    super_block sb;
    read_super_block(&sb);
    for (size_t idx = 0; idx < INODES_CNT; ++idx) {
        if (!sb.inodes[idx].used) {
            continue;
        }
        if (strcmp(name, sb.inodes[idx].name)) {
            continue;
        }
        if (block_num >= sb.inodes[idx].size) {
            silent_error(INVALID_BLOCK_READ);
            return -1;
        }
        block b;
        read_block_data(sb.inodes[idx].block_pointers[block_num], &b);
        strcpy(buf, b.content);
        return 0;
    }
    silent_error(NO_FILE);
    return -1;
} 


int write_block(char name[MAX_FILENAME_LEN], int block_num, char buf[BLOCK_SIZE]) {
    super_block sb;
    read_super_block(&sb);
    for (size_t idx = 0; idx < INODES_CNT; ++idx) {
        if (!sb.inodes[idx].used) {
            continue;
        }
        if (strcmp(name, sb.inodes[idx].name)) {
            continue;
        }
        if (block_num >= sb.inodes[idx].size) {
            silent_error(INVALID_BLOCK_WRITE);
            return -1;
        }
        block b;
        read_block_data(sb.inodes[idx].block_pointers[block_num], &b);
        strcpy(b.content, buf);
        write_block_data(sb.inodes[idx].block_pointers[block_num], &b);
        return 0;
    }
    silent_error(NO_FILE);
    return -1;
} 

int main(int argc, char *argv[]) {
    if (argc != 2) {
        raise_error(NO_ARGS_ERROR);
    }
    
    assert(sizeof(super_block) == sizeof(block));
    FILE* file = fopen(argv[1], "r");

    fscanf(file, "%s", DISK_NAME);
    disk = fopen(DISK_NAME, "r+");

    int read_idx = 0;
    while (!feof(file)) {
        char op_type;
        fscanf(file, "%c", &op_type);
        if (op_type == 'C') {
            char name[MAX_FILENAME_LEN];
            int size;
            fscanf(file, "%s%d", name, &size);
            create(name, size);
        } else if (op_type == 'D') {
            char name[MAX_FILENAME_LEN];
            fscanf(file, "%s", name);
            delete(name);
        } else if (op_type == 'L') {
            ls();
        } else if (op_type == 'W') {
            char name[MAX_FILENAME_LEN];
            int block_num;
            char buf[BLOCK_SIZE];
            sprintf(buf, "Data write operation #%d", ++read_idx);
            fscanf(file, "%s%d", name, &block_num);
            write_block(name, block_num, buf);
        } else if (op_type == 'R') {
            char name[MAX_FILENAME_LEN];
            int block_num;
            char buf[BLOCK_SIZE];
            fscanf(file, "%s%d", name, &block_num);
            if (read_block(name, block_num, buf) == 0) {
                printf("Read data \"%s\"\n", buf);
            }
        }
    }
    return 0;
}

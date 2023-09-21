#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_FILENAME_LENGTH 64
#define MAX_PATH_LENGTH 2049
#define MAX_DATA_SIZE 1025
#define MAX_SUBDIRS 255
#define MAX_FILES 255

struct File;
struct Directory;

typedef unsigned long int fid_t;
typedef unsigned long int size_t;

struct File;
struct Directory;

typedef struct File File;
typedef struct Directory Directory;

fid_t f_id = 0;

struct File {
    fid_t id;
    char name[MAX_FILENAME_LENGTH];
    size_t size;
    char data[MAX_DATA_SIZE];
    Directory* directory;
};

struct Directory {
    char name[MAX_FILENAME_LENGTH];
    File* files[MAX_FILES];
    Directory* directories[MAX_SUBDIRS];
    unsigned char nf;
    unsigned char nd;
    char path[MAX_PATH_LENGTH];
};

File* touch(Directory* dir, const char* name) {
    File* file = (File*) malloc(sizeof(File));
    file->id = ++f_id;
    strcpy(file->name, name);
    file->size = 0;
    file->directory = dir;
    dir->files[dir->nf++] = file;
    return file;
}

Directory* mkdir(const char* name) {
    Directory* dir = (Directory*) malloc(sizeof(Directory));
    strcpy(dir->name, name);
    strcpy(dir->path, name);
    dir->nf = dir->nd = 0;
    return dir;
}

void add_dir(Directory* dir1, Directory* dir2) {
    if (!dir1 || !dir2) {
        return;
    }
    dir2->directories[dir2->nd] = dir1;
    ++dir2->nd;
    char temp[MAX_PATH_LENGTH];
    if (strcmp(dir2->path, "/")) {
        strcpy(temp, dir2->path);
        strcat(temp, "/");
        strcat(temp, dir1->name);
        strcpy(dir1->path, temp);
    } else {
        strcpy(temp, "/");
        strcat(temp, dir1->name);
        strcpy(dir1->path, temp);
    }
}

void show_file(File *file) {
    printf("%s ", file->name);
}

void show_dir(Directory *dir) {
    printf("\nDIRECTORY\n");
    printf(" path: %s\n", dir->path);
    printf(" files:\n");
    printf(" [ ");
    for (int i = 0; i < dir->nf; ++i) {
        show_file(dir->files[i]);
    }
    printf("]\n");
    printf(" directories:\n");
    printf(" { ");
    for (int i = 0; i < dir->nd; ++i) {
        show_dir(dir->directories[i]);
    }
    printf("}\n");
}

void overwrite_to_file(File* file, const char* str) {
   strcpy(file->data, str);
   file->size = sizeof(file->data);
}

void append_to_file(File* file, const char* str) {
   strcat(file->data, str);
   file->size = sizeof(file->data);
}

void printp_file(File* file) {
    char temp_path[MAX_PATH_LENGTH];
    strcpy(temp_path, file->directory->path);
    strcat(temp_path, "/");
    strcat(temp_path, file->name);
    printf("%s\n", temp_path);
}

void print_inner_files(Directory* dir) {
    for (size_t idx = 0; idx < dir->nf; ++idx) {
        printp_file(dir->files[idx]);
    }
    for (size_t idx = 0; idx < dir->nd; ++idx) {
        print_inner_files(dir->directories[idx]);
    }
}

int main() {
    Directory* root = mkdir("/");
    Directory* bin = mkdir("bin");
    Directory* home = mkdir("home");
    
    add_dir(home, root);
    add_dir(bin, root);
    
    File* bash = touch(bin, "bash");
    File* ex3_1 = touch(home, "ex3_1.c");
    File* ex3_2 = touch(home, "ex3_2.c");
    
    overwrite_to_file(ex3_1, "int printf(const char * format, ...);");
    overwrite_to_file(ex3_2, "This is a comment in C language");
    overwrite_to_file(bash, "Bourne Again Shell!!!"); 
    append_to_file(ex3_1, "int main(){printf(”Hello World!”)}");
    
    print_inner_files(root);   

    free(root);
    free(bin);
    free(home);
    free(ex3_1);
    free(ex3_2);
    free(bash);
    return 0;
}

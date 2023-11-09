#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>

#define raise_error(ERROR) do { \
    printf(ERROR);              \
    exit(-1); } while (0) 

#define MAX_PATH_LEN 100

#define INVALID_ARGS "Invalid number of arguments\n"
#define DIR_OPEN_ERROR "Error occured while directory opening\n"
#define UNLINK_ERROR "Error occured while unlinking\n"
#define LSTAT_ERROR "Error while lstat\n"
#define SYMLINK_ERROR "Error occured while symlink creation\n"
#define HARD_LINK_ERROR "Error occured while hard link creation\n"
#define MOVE_ERROR "Error occured while moving file\n"
#define REAL_PATH_ERROR "Error occured during realpath() call\n"

char* path;

void join_path_name(const char* pth, const char* name, char* res) {
    strcpy(res, pth);
    strcat(res, "/");
    strcat(res, name);
}

void print_stat(const struct stat *st) {
    printf("File type:                 %o\n", (unsigned int)st->st_mode);
    printf("File size:                 %lld bytes\n", (long long)st->st_size);
//    printf("Device ID:                 %d\n", (int)st->st_dev);
    printf("Inode number:              %lu\n", (unsigned long)st->st_ino);
    printf("Number of hard links:      %u\n", (unsigned int)st->st_nlink);
//    printf("UID of the file owner:     %u\n", (unsigned int)st->st_uid);
//    printf("GID of the file owner:     %u\n", (unsigned int)st->st_gid);
//    printf("File's device ID:          %d\n", (int)st->st_rdev);
//    printf("Block size for filesystem: %ld bytes\n", (long)st->st_blksize);
//    printf("Number of 512B blocks:     %lld\n", (long long)st->st_blocks);
    printf("Last status change time:   %s", ctime(&st->st_ctime));
    printf("Last file modification:    %s", ctime(&st->st_mtime));
    printf("Last file access:          %s", ctime(&st->st_atime));
}

void print_file_info(const char* filepath) {
    struct stat info;
    if (stat(filepath, &info) != 0) {
        return;
    }
    printf("Stats for the file/directory \"%s\"\n", filepath);
    print_stat(&info);
}

ino_t get_inode(const char* filepath, const char* filename) {
    char temp_path[MAX_PATH_LEN];
    join_path_name(filepath, filename, temp_path);
    struct stat info;
    if (lstat(temp_path, &info) != 0) {
        raise_error(LSTAT_ERROR);
    }
    return info.st_ino;
}

void print_contents(const char* path) {
    FILE* file = fopen(path, "r");
    if (file == NULL) {
        printf("Did't access the content\n");
        return;
    }
    int c;
    while ((c = fgetc(file)) != EOF) {
        putchar(c);
    }
    putchar('\n');
    fclose(file);
}

void create_file(const char* filepath) {
    FILE* file = fopen(filepath, "w");
    fclose(file);
} 

void write_to_file(const char* filepath, const char* content) {
    FILE* file  = fopen(filepath, "w");
    fprintf(file, "%s", content);
    fclose(file);
}

void recursive_find_all_hlinks(const char* cur_path, ino_t src_ino) {
    DIR* dir;
    if (((dir = opendir(cur_path)) == NULL)) {
        raise_error(DIR_OPEN_ERROR);
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        char temp_path[MAX_PATH_LEN];
        join_path_name(cur_path, entry->d_name, temp_path);
        
        struct stat info;
        if (lstat(temp_path, &info) != 0) {
            continue;
        }

        if (S_ISLNK(info.st_mode)) {
            continue;
        }

        if (S_ISDIR(info.st_mode)) {
            if (strcmp(entry->d_name, ".") != 0 && 
                    strcmp(entry->d_name, "..") != 0) {
                recursive_find_all_hlinks(temp_path, src_ino);
            }
        } else if (S_ISREG(info.st_mode)) {
            if (info.st_ino == src_ino) {
                char absolute_path[MAX_PATH_LEN];
                if (realpath(temp_path, absolute_path) == NULL) {
                    raise_error(REAL_PATH_ERROR);
                }
                printf("Hard link \"%s\" with inode %lu with path \"%s\"\n", 
                        entry->d_name, info.st_ino, absolute_path);
                print_contents(temp_path);
            }
        }
    }
}

void find_all_hlinks(const char* source) {
    ino_t src_inode = get_inode(path, source);
    recursive_find_all_hlinks(path, src_inode);
}

void recursive_unlink_all(
        const char* cur_path, 
        ino_t src_ino, 
        bool* found, 
        char* remaining_link) {
    DIR* dir;
    if (((dir = opendir(cur_path)) == NULL)) {
        raise_error(DIR_OPEN_ERROR);
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        char temp_path[MAX_PATH_LEN];
        join_path_name(cur_path, entry->d_name, temp_path);
        struct stat info;
        if (lstat(temp_path, &info) != 0) {
            continue;
        }

        if (S_ISLNK(info.st_mode)) {
            continue;
        }
        if (S_ISDIR(info.st_mode)) {
            if (strcmp(entry->d_name, ".") != 0 && 
                    strcmp(entry->d_name, "..") != 0) {
                recursive_unlink_all(temp_path, src_ino, found, remaining_link);
            }
        } else if (S_ISREG(info.st_mode)) {
            if (info.st_ino == src_ino) {
                if (*found) {
                    if (unlink(temp_path) != 0) {
                        raise_error(UNLINK_ERROR);
                    }
                } else {
                    *found = true;
                    join_path_name(cur_path, entry->d_name, remaining_link);
                }
            }
        }
    }
}

void unlink_all(const char* source) {
    char temp_path[MAX_PATH_LEN];
    join_path_name(path, source, temp_path);
    struct stat info;
    if (lstat(temp_path, &info) != 0) {
        raise_error(LSTAT_ERROR);
    }

    char remaining_link[MAX_PATH_LEN];
    bool found = false;
    recursive_unlink_all(path, info.st_ino, &found, remaining_link);
    printf("Last hard link to source \"%s\" is \"%s\"\n", source, remaining_link);
    print_file_info(remaining_link);
}

void create_sym_link(const char* source, const char* linkname) {
    if (symlink(source, linkname) != 0) {
        raise_error(SYMLINK_ERROR);
    }
}

void create_hard_link(const char* source, const char* linkname) {
    if (link(source, linkname) != 0) {
        raise_error(HARD_LINK_ERROR);
    }
}


void move(const char* src, const char* dest) {
    if (rename(src, dest) != 0) {
        raise_error(MOVE_ERROR);
    } 
}
int main(int argc, char* argv[]) {
    if (argc != 2) {
        raise_error(INVALID_ARGS);
    }
    path = argv[1];

    char src[MAX_PATH_LEN];
    join_path_name(path, "myfile1.txt", src);

    create_file(src);
    usleep(5);
    write_to_file(src, "Hello world.");

    char link1[MAX_PATH_LEN];
    join_path_name(path, "myfile11.txt", link1);
    usleep(5);
    create_hard_link(src, link1);

    usleep(5);

    char link2[MAX_PATH_LEN];
    join_path_name(path, "myfile12.txt", link2);
    create_hard_link(src, link2);
    usleep(5);
    find_all_hlinks(src);
    
    char new_dest[MAX_PATH_LEN];
    strcpy(new_dest, "/tmp");
    //join_path_name(path, "tmp", new_dest);
    mkdir(new_dest, 0777);
    strcat(new_dest, "/myfile1.txt");
    usleep(5);
    move(src, new_dest);
    usleep(5);
    write_to_file(link1, "Something new!");

    char symlink1[MAX_PATH_LEN];
    join_path_name(path, "myfile13.txt", symlink1);
    usleep(5);
    create_sym_link(new_dest, symlink1);

    usleep(5);
    write_to_file(new_dest, "Something new! Again!");
    usleep(5);
    unlink_all("myfile11.txt");
}

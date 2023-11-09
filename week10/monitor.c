#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <stdbool.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <linux/inotify.h>

#define raise_error(ERROR) do { \
    printf(ERROR);              \
    exit(-1); } while (0) 

#define MAX_PATH_LEN 100

#define MONITORING_OPTIONS (IN_ACCESS | IN_CREATE | IN_DELETE | IN_MODIFY | IN_OPEN | IN_ATTRIB)

#define LINE "________________________________\n"

#define INVALID_ARGS "Invalid number of arguments\n"
#define DIR_OPEN_ERROR "Error occured while directory opening\n"
#define INOTIFY_INIT_ERROR "Error occured during inotify init\n"
#define MONITORING_ERROR "Error occured while monitoring\n"

#define EVENT_SIZE (sizeof(struct inotify_event))
#define EVENT_BUF_LEN (1024 * (EVENT_SIZE + 16))

char* path;
int monitor, directory;

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
    printf(LINE);
    printf("Stats for the file/directory \"%s\"\n", filepath);
    print_stat(&info);
}

void print_info_on_path() {
    DIR* dir;
    if (((dir = opendir(path)) == NULL)) {
        raise_error(DIR_OPEN_ERROR);
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') {
            continue;
        }

        char temp_path[MAX_PATH_LEN];
        join_path_name(path, entry->d_name, temp_path);
        print_file_info(temp_path);    
    }
    closedir(dir);
}

void handle_termination(int sgn) {
    print_info_on_path();
    close(monitor);
    inotify_rm_watch(monitor, directory);
    exit(sgn);
}

void monitoring() {
    int events_seq_len = 0, idx = 0;
    monitor = inotify_init();
    if (monitor < 0) {
        raise_error(INOTIFY_INIT_ERROR);
    }
    directory = inotify_add_watch(monitor, path, MONITORING_OPTIONS);
   
    char buffer[EVENT_BUF_LEN];
    while (true) {
        events_seq_len = read(monitor, buffer, EVENT_BUF_LEN);
        if (events_seq_len < 0) {
            raise_error(MONITORING_ERROR);
        }
        struct inotify_event *event = (struct inotify_event*) &buffer[idx];
        if (event->len == 0 || !(event->mask & (MONITORING_OPTIONS))) {
            continue;
        }
        const char* filetype = (event->mask & IN_ISDIR ? "Directory" : "File");
        char temp_path[MAX_PATH_LEN];
        join_path_name(path, event->name, temp_path);
        if (event->mask & IN_CREATE) {
            printf(LINE);
            printf("%s \"%s\" was created\n", filetype, event->name);
            print_file_info(temp_path);
        } else if (event->mask & IN_DELETE) { 
            printf(LINE);
            printf("%s \"%s\" was deleted\n", filetype, event->name);
        } else if (event->mask & IN_OPEN) {
            printf(LINE);
            printf("%s \"%s\" was opened\n", filetype, event->name);
            print_file_info(temp_path);
        } else if (event->mask & IN_MODIFY) {
            printf(LINE);
            printf("%s \"%s\" was modified\n", filetype, event->name);
            print_file_info(temp_path);
        } else if (event->mask & IN_ACCESS) {
            printf(LINE);
            printf("%s \"%s\" was accessed\n", filetype, event->name);
            print_file_info(temp_path);
        } else if (event->mask & IN_ATTRIB) {
            printf(LINE);
            printf("%s's \"%s\" metadata was updated\n", filetype, event->name);
            print_file_info(temp_path);
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        raise_error(INVALID_ARGS);
    }
    path = argv[1];
    
    signal(SIGINT, handle_termination);
    
    print_info_on_path();
    monitoring();
}

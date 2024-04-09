#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#define MAX_ENTRIES 1000 
#define PATH_MAX 1000


struct EntryMetadata {
    char name[256]; 
    mode_t mode;    
    off_t size;     
    time_t mtime;   
};


void captureSnapshot(const char* directory, struct EntryMetadata snapshot[], int* num_entries) {
    DIR* dir;
    struct dirent *entry;
    struct stat file_info;

    dir = opendir(directory);
    if (!dir) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }
    *num_entries = 0;   
    while ((entry = readdir(dir)) != NULL && *num_entries < MAX_ENTRIES) {
        
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        char path[PATH_MAX];
        snprintf(path, sizeof(path), "%s/%s", directory, entry->d_name);
        if (lstat(path, &file_info) == -1) {
            perror("Error getting file info");
            continue;
        }
        strncpy(snapshot[*num_entries].name, entry->d_name, sizeof(snapshot[*num_entries].name));
        snapshot[*num_entries].mode = file_info.st_mode;
        snapshot[*num_entries].size = file_info.st_size;
        snapshot[*num_entries].mtime = file_info.st_mtime;
        (*num_entries)++;
    }

    closedir(dir);
}
int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <directory>\n", argv[0]);
        return 1;
    }
    const char* directory = argv[1];
    struct EntryMetadata snapshot_before[MAX_ENTRIES];
    struct EntryMetadata snapshot_after[MAX_ENTRIES];
    int num_entries_before, num_entries_after;

    captureSnapshot(directory, snapshot_before, &num_entries_before);

    while (1) {
        captureSnapshot(directory, snapshot_after, &num_entries_after);
        memcpy(snapshot_before, snapshot_after, sizeof(snapshot_before));
        num_entries_before = num_entries_after;
        sleep(10); 
    }

    return 0;
}

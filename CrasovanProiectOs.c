#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h> 
#include <sys/wait.h> 
#include <time.h>
#include <sys/types.h>

#define PATH_MAX 1000

struct EntryMetadata {
    char name[256];
    mode_t mode;
    off_t size;
    time_t mtime;
};

void captureSnapshot(const char* directory, const char* output_dir) {
    DIR *dir = opendir(directory);
    if (!dir) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }    

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char path[PATH_MAX];
        snprintf(path, sizeof(path), "%s/%s", directory, entry->d_name);

        struct stat file_stat;
        if (stat(path, &file_stat) == -1) {
            perror("Error getting file info");
            continue;
        }

        // Create EntryMetadata struct to hold file metadata
        struct EntryMetadata metadata;
        strncpy(metadata.name, entry->d_name, sizeof(metadata.name));
        metadata.mode = file_stat.st_mode;
        metadata.size = file_stat.st_size;
        metadata.mtime = file_stat.st_mtime;

        // Create file name for the snapshot
        char snapshot_filename[PATH_MAX];
        snprintf(snapshot_filename, sizeof(snapshot_filename), "%s/%s_snapshot.txt", output_dir, entry->d_name);

        // Open file for writing
        FILE *snapshot_file = fopen(snapshot_filename, "w");
        if (!snapshot_file) {
            perror("Error creating snapshot file");
            continue;
        }

        // Write metadata to file
        fprintf(snapshot_file, "File Name: %s\n", metadata.name);
        fprintf(snapshot_file, "File Mode: %o\n", metadata.mode);
        fprintf(snapshot_file, "File Size: %lld bytes\n", (long long)metadata.size);
        fprintf(snapshot_file, "File Modification Time: %s", ctime(&metadata.mtime));

        // Close file
        fclose(snapshot_file);

        printf("Snapshot for file %s created successfully: %s\n", entry->d_name, snapshot_filename);
    }
    
    closedir(dir);
    printf("Snapshots for Directory %s created successfully.\n", directory);
}


void performMissingPermissionsVerification(const char* directory, const char* isolated_space_dir) {
    DIR* dir;
    struct dirent *entry;

    dir = opendir(directory);
    if (!dir) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char path[PATH_MAX];
        snprintf(path, sizeof(path), "%s/%s", directory, entry->d_name);

        struct stat file_stat;
        if (stat(path, &file_stat) == -1) {
            perror("Error getting file info");
            continue;
        }

        if ((file_stat.st_mode & S_IRWXU) == 0 && (file_stat.st_mode & S_IRWXG) == 0 && (file_stat.st_mode & S_IRWXO) == 0) {
            pid_t pid = fork();

            if (pid == -1) {
                perror("Error forking process");
                continue;
            } else if (pid == 0) {
                
                execl("/bin/bash", "bash", "verify_for_malicious.sh", path, isolated_space_dir, NULL);
                perror("Error executing script");
                exit(EXIT_FAILURE);
            }
        }
    }

    closedir(dir);
}

void performSyntacticAnalysis(const char* file_path) {
  
    printf("Syntactic analysis for file %s performed.\n", file_path);
}

void isolateDangerousFiles(const char* source_path, const char* destination_path) {
   
    printf("File %s moved to isolation directory %s.\n", source_path, destination_path);
}

int main(int argc, char* argv[]) {
    if (argc < 5 || argc > 15) {
        printf("Usage: %s -o output_dir -s isolated_space_dir dir1 dir2 ... dirN\n", argv[0]);
        return 1;
    }

    const char* output_dir = NULL;
    const char* isolated_space_dir = NULL;

   
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            output_dir = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
            isolated_space_dir = argv[i + 1];
            i++;
        }
    }

    if (output_dir == NULL || isolated_space_dir == NULL) {
        printf("Output directory or isolated space directory not specified.\n");
        return 1;
    }

    
    for (int i = 4; i < argc; i++) {
        captureSnapshot(argv[i], output_dir);
    }

   
    for (int i = 4; i < argc; i++) {
        performMissingPermissionsVerification(argv[i], isolated_space_dir);
    }

   
    for (int i = 4; i < argc; i++) {
    }

    return 0;
}

#include "add.h"
#include "utils.h"
#include "hash.h"
#include "guard.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

static int add_file(const char *filename) {
    if (is_ignored(filename)) {
        return 0; 
    }

    if (!path_exists(filename)) {
        fprintf(stderr, "Error: File '%s' not found.\n", filename);
        return 1;
    }

    
    char dest[512];
    build_path(dest, sizeof(dest), STAGING_DIR, filename);

    if (copy_file_with_dirs(filename, dest) != 0) {
        fprintf(stderr, "Error: Failed to stage '%s'.\n", filename);
        return 1;
    }

    /* Compute hash of the staged file */
    char hash[17];
    hash_file(filename, hash);

    
    char *existing = read_file(INDEX_FILE);
    char new_index[8192] = "";

    if (existing) {
        char *line = strtok(existing, "\n");
        while (line) {
            char name[256] = "";
            sscanf(line, "%255[^|]", name);
            if (strcmp(name, filename) != 0 && strlen(name) > 0) {
                strncat(new_index, line, sizeof(new_index) - strlen(new_index) - 2);
                strncat(new_index, "\n",  sizeof(new_index) - strlen(new_index) - 1);
            }
            line = strtok(NULL, "\n");
        }
        free(existing);
    }

    /* Append the new entry for this file */
    char entry[600];
    snprintf(entry, sizeof(entry), "%s|%s\n", filename, hash);
    strncat(new_index, entry, sizeof(new_index) - strlen(new_index) - 1);

    write_file(INDEX_FILE, new_index);

    printf("Staged: %s\n", filename);
    return 0;
}

static int add_recursive(const char *path) {
    if (is_directory(path)) {
        DIR *dir = opendir(path);
        if (!dir) return 1;

        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;

            if (strcmp(entry->d_name, ".chronicle") == 0 || strcmp(entry->d_name, ".git") == 0)
                continue;

            char full_path[512];

            if (strcmp(path, ".") == 0) {
                snprintf(full_path, sizeof(full_path), "%s", entry->d_name);
            } else {
                snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
            }

            if (is_ignored(full_path)) {
                continue;
            }
            
            add_recursive(full_path);
        }
        closedir(dir);
        return 0;
    } else {
        return add_file(path);
    }
}

int cmd_add(const char *filename) {
    if (!path_exists(CHRONICLE_DIR)) {
        fprintf(stderr, "Error: Not a Chronicle repository. Run 'chronicle init' first.\n");
        return 1;
    }

    return add_recursive(filename);
}

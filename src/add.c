#include "add.h"
#include "utils.h"
#include "hash.h"
#include "guard.h"
#include <stdio.h>

int cmd_add(const char *filename) {
    if (!path_exists(CHRONICLE_DIR)) {
        fprintf(stderr, "Error: Not a Chronicle repository. Run 'chronicle init' first.\n");
        return 1;
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

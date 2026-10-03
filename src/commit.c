#include "commit.h"
#include "utils.h"
#include "guard.h"
#include <stdio.h>

int cmd_commit(const char *message) {
    if (!path_exists(CHRONICLE_DIR)) {
        fprintf(stderr, "Error: Not a Chronicle repository.\n");
        return 1;
    }

    /* Read the staging index to get files queued for commit */
    char *index = read_file(INDEX_FILE);
    if (!index || strlen(index) == 0) {
        printf("Nothing to commit. Stage files with 'chronicle add <file>'.\n");
        free(index);
        return 0;
    }

    /* Chronicle Guard */
    int violations = check_guards();
    if (violations > 0) {
        printf("\nProceed with commit anyway? (y/n): ");
        char response[4];
        if (!fgets(response, sizeof(response), stdin) || response[0] != 'y') {
            printf("Commit aborted.\n");
            free(index);
            return 1;
        }
    }


    int new_id   = get_head() + 1;
    char commit_path[256];
    char files_path[256];
    snprintf(commit_path, sizeof(commit_path), "%s/%04d",       COMMITS_DIR, new_id);
    snprintf(files_path,  sizeof(files_path),  "%s/%04d/files", COMMITS_DIR, new_id);

    if (make_dir(commit_path) != 0 || make_dir(files_path) != 0) {
        fprintf(stderr, "Error: Could not create commit directory.\n");
        free(index);
        return 1;
    }

    char file_list[4096] = "";  
    char *index_copy = strdup(index);
    char *line = strtok(index_copy, "\n");

    while (line) {
        char name[256] = "";
        sscanf(line, "%255[^|]", name);

        if (strlen(name) == 0) {
            line = strtok(NULL, "\n");
            continue;
        }

        char staged[512], snapshot[512];
        build_path(staged,   sizeof(staged),   STAGING_DIR, name);
        build_path(snapshot, sizeof(snapshot), files_path,  name);

        if (copy_file_with_dirs(staged, snapshot) != 0) {
            fprintf(stderr, "Error: Could not snapshot '%s'.\n", name);
        }

        if (strlen(file_list) > 0)
            strncat(file_list, ",", sizeof(file_list) - strlen(file_list) - 1);
        strncat(file_list, name, sizeof(file_list) - strlen(file_list) - 1);

        line = strtok(NULL, "\n");
    }
    free(index_copy);

    char timestamp[32];
    get_timestamp(timestamp, sizeof(timestamp));

    char meta_path[256];
    build_path(meta_path, sizeof(meta_path), commit_path, "meta.txt");

    FILE *meta = fopen(meta_path, "w");
    if (!meta) {
        fprintf(stderr, "Error: Could not write commit metadata.\n");
        free(index);
        return 1;
    }
    fprintf(meta, "id=%d\n",        new_id);
    fprintf(meta, "message=%s\n",   message);
    fprintf(meta, "timestamp=%s\n", timestamp);
    fprintf(meta, "parent=%d\n",    new_id - 1);
    fprintf(meta, "files=%s\n",     file_list);
    fclose(meta);

    set_head(new_id);
    write_file(INDEX_FILE, "");

    printf("[%04d] %s\n", new_id, message);
    printf("  Committed at %s\n", timestamp);

    free(index);
    return 0;
}

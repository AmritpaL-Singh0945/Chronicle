#include "rollback.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

int cmd_rollback(int commit_id) {
    if (!path_exists(CHRONICLE_DIR)) {
        fprintf(stderr, "Error: Not a Chronicle repository.\n");
        return 1;
    }

    char commit_path[256], meta_path[256], files_path[256];
    snprintf(commit_path, sizeof(commit_path), "%s/%04d",       COMMITS_DIR, commit_id);
    snprintf(meta_path,   sizeof(meta_path),   "%s/%04d/meta.txt", COMMITS_DIR, commit_id);
    snprintf(files_path,  sizeof(files_path),  "%s/%04d/files", COMMITS_DIR, commit_id);

    if (!path_exists(commit_path)) {
        fprintf(stderr, "Error: Commit %04d does not exist.\n", commit_id);
        return 1;
    }

    FILE *meta = fopen(meta_path, "r");
    if (!meta) {
        fprintf(stderr, "Error: Cannot read commit metadata.\n");
        return 1;
    }

    char line[512];
    char file_list[4096] = "";
    char msg[256] = "";
    while (fgets(line, sizeof(line), meta)) {
        if (strncmp(line, "files=", 6) == 0) {
            strncpy(file_list, line + 6, sizeof(file_list) - 1);
            file_list[strcspn(file_list, "\n")] = '\0';
        } else if (strncmp(line, "message=", 8) == 0) {
            strncpy(msg, line + 8, sizeof(msg) - 1);
            msg[strcspn(msg, "\n")] = '\0';
        }
    }
    fclose(meta);

    printf("Rolling back to commit %04d: \"%s\"\n", commit_id, msg);
    printf("This will overwrite current files. Continue? (y/n): ");
    char response[4];
    if (!fgets(response, sizeof(response), stdin) || response[0] != 'y') {
        printf("Rollback cancelled.\n");
        return 0;
    }

    char *token = strtok(file_list, ",");
    while (token) {
        char snapshot[512];
        build_path(snapshot, sizeof(snapshot), files_path, token);

        if (copy_file_with_dirs(snapshot, token) == 0) {
            printf("  Restored: %s\n", token);
        } else {
            fprintf(stderr, "  Failed to restore: %s\n", token);
        }
        token = strtok(NULL, ",");
    }

    write_file(INDEX_FILE, "");

    printf("\nRollback complete. Working tree is now at commit %04d.\n", commit_id);
    printf("(HEAD still points to %04d — commit again if you want to record this state)\n", get_head());
    return 0;
}

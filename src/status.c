#include "status.h"
#include "utils.h"
#include "hash.h"
#include <stdio.h>
#include <string.h>

int cmd_status(void) {
    if (!path_exists(CHRONICLE_DIR)) {
        fprintf(stderr, "Error: Not a Chronicle repository.\n");
        return 1;
    }

    printf("\n  Chronicle Status\n");
    printf("  ─────────────────────────────────────────\n");

    int head = get_head();

    /* ── Section 1: Files staged and ready to commit ── */
    char *index = read_file(INDEX_FILE);
    int staged_count = 0;

    if (index && strlen(index) > 0) {
        printf("  Changes staged for commit:\n");

        char *copy = strdup(index);
        char *line = strtok(copy, "\n");
        while (line) {
            char name[256] = "", staged_hash[17] = "";
            sscanf(line, "%255[^|]|%16s", name, staged_hash);

            if (strlen(name) == 0) { line = strtok(NULL, "\n"); continue; }

            /* Check if this file differs from what's in the last commit */
            char committed_hash[17] = "none";
            if (head > 0) {
                char committed_path[512];
                snprintf(committed_path, sizeof(committed_path),
                         "%s/%04d/files/%s", COMMITS_DIR, head, name);
                if (path_exists(committed_path))
                    hash_file(committed_path, committed_hash);
            }

            if (strcmp(staged_hash, committed_hash) != 0) {
                printf("      new/modified: %s\n", name);
            } else {
                printf("      unchanged:     %s\n", name);
            }
            staged_count++;
            line = strtok(NULL, "\n");
        }
        free(copy);
    }

    if (staged_count == 0)
        printf("  Nothing staged. Use 'chronicle add <file>' to stage changes.\n");

    /* ── Section 2: Staged files modified again since staging ── */
    printf("\n  Changes in working tree (not yet staged):\n");

    int unstaged_count = 0;
    if (index && strlen(index) > 0) {
        char *copy2 = strdup(index);
        char *line = strtok(copy2, "\n");
        while (line) {
            char name[256] = "", staged_hash[17] = "";
            sscanf(line, "%255[^|]|%16s", name, staged_hash);
            if (strlen(name) == 0) { line = strtok(NULL, "\n"); continue; }

            if (!path_exists(name)) {
                printf("      deleted:   %s\n", name);
                unstaged_count++;
            } else {
                char current_hash[17];
                hash_file(name, current_hash);
                if (strcmp(current_hash, staged_hash) != 0) {
                    printf("      modified:  %s\n", name);
                    unstaged_count++;
                }
            }
            line = strtok(NULL, "\n");
        }
        free(copy2);
    }

    if (unstaged_count == 0)
        printf("  (none)\n");

    printf("  ─────────────────────────────────────────\n");
    if (staged_count > 0)
        printf("  Run 'chronicle commit -m <message>' to commit.\n");

    free(index);
    return 0;
}

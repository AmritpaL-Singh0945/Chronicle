#include "guard.h"
#include "utils.h"
#include "hash.h"
#include <stdio.h>
#include <string.h>

int cmd_guard(const char *filename) {
    if (!path_exists(CHRONICLE_DIR)) {
        fprintf(stderr, "Error: Not a Chronicle repository.\n");
        return 1;
    }

    /* Don't guard a file that doesn't exist */
    if (!path_exists(filename)) {
        fprintf(stderr, "Error: '%s' not found in working directory.\n", filename);
        return 1;
    }

    /* Check if already guarded */
    if (is_guarded(filename)) {
        printf("'%s' is already guarded.\n", filename);
        return 0;
    }

    /* Append filename to guards.cfg */
    FILE *f = fopen(GUARDS_FILE, "a");
    if (!f) {
        fprintf(stderr, "Error: Cannot update guards file.\n");
        return 1;
    }
    fprintf(f, "%s\n", filename);
    fclose(f);

    printf("  [GUARD ENABLED] '%s' is now protected.\n", filename);
    printf("  Chronicle will warn you if this file goes missing before a commit.\n");
    return 0;
}

int is_guarded(const char *filename) {
    FILE *f = fopen(GUARDS_FILE, "r");
    if (!f) return 0;

    char line[256];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, filename) == 0) {
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

int check_guards(void) {
    FILE *f = fopen(GUARDS_FILE, "r");
    if (!f) return 0;

    int violations = 0;
    char line[256];

    int head = get_head();

    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        if (strlen(line) == 0) continue;

        if (!path_exists(line)) {
            if (violations == 0) {
                printf("\n");
                printf("  ╔══════════════════════════════════════════╗\n");
                printf("  ║        !! CHRONICLE GUARD WARNING !!      ║\n");
                printf("  ╚══════════════════════════════════════════╝\n");
            }
            printf("  MISSING guarded file:  '%s'\n", line);
            violations++;
        } else if (head > 0) {
            /* Check if it was modified since the last commit */
            char snapshot_path[512];
            snprintf(snapshot_path, sizeof(snapshot_path), "%s/%04d/files/%s", COMMITS_DIR, head, line);

            if (path_exists(snapshot_path)) {
                char current_hash[17], snap_hash[17];
                hash_file(line, current_hash);
                hash_file(snapshot_path, snap_hash);

                if (strcmp(current_hash, snap_hash) != 0) {
                    if (violations == 0) {
                        printf("\n");
                        printf("  ╔══════════════════════════════════════════╗\n");
                        printf("  ║        !! CHRONICLE GUARD WARNING !!      ║\n");
                        printf("  ╚══════════════════════════════════════════╝\n");
                    }
                    printf("  MODIFIED guarded file: '%s'\n", line);
                    violations++;
                }
            }
        }
    }

    if (violations > 0) {
        printf("  %d guarded file(s) have been deleted or modified.\n", violations);
        printf("  Make sure these changes are intentional before committing.\n\n");
    }

    fclose(f);
    return violations;
}

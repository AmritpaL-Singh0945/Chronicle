#include "guard.h"
#include "utils.h"
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
            printf("  MISSING guarded file: '%s'\n", line);
            violations++;
        }
    }

    if (violations > 0) {
        printf("  %d guarded file(s) are missing from your working directory.\n", violations);
        printf("  Make sure you haven't accidentally deleted them.\n\n");
    }

    fclose(f);
    return violations;
}

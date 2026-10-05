#include "diff.h"
#include "utils.h"
#include "hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES    2000
#define MAX_LINE_LEN 1024

static int read_lines(const char *path, char lines[][MAX_LINE_LEN]) {
    FILE *f = fopen(path, "r");
    if (!f) return 0;

    int count = 0;
    while (count < MAX_LINES && fgets(lines[count], MAX_LINE_LEN, f)) {
        int len = strlen(lines[count]);
        if (len > 0 && lines[count][len - 1] == '\n')
            lines[count][len - 1] = '\0';
        count++;
    }
    fclose(f);
    return count;
}

void diff_files(const char *old_path, const char *new_path, const char *label) {
    char (*old_lines)[MAX_LINE_LEN] = malloc(MAX_LINES * MAX_LINE_LEN);
    char (*new_lines)[MAX_LINE_LEN] = malloc(MAX_LINES * MAX_LINE_LEN);
    if (!old_lines || !new_lines) {
        fprintf(stderr, "Error: Out of memory during diff.\n");
        free(old_lines); free(new_lines);
        return;
    }

    int old_count = read_lines(old_path, old_lines);
    int new_count = read_lines(new_path, new_lines);

    int **lcs = (int **)malloc((old_count + 1) * sizeof(int *));
    for (int i = 0; i <= old_count; i++) {
        lcs[i] = (int *)calloc(new_count + 1, sizeof(int));
    }

    /* Fill the LCS table bottom-up */
    for (int i = 1; i <= old_count; i++) {
        for (int j = 1; j <= new_count; j++) {
            if (strcmp(old_lines[i - 1], new_lines[j - 1]) == 0) {
                lcs[i][j] = lcs[i - 1][j - 1] + 1;
            } else {
                lcs[i][j] = lcs[i - 1][j] > lcs[i][j - 1]
                           ? lcs[i - 1][j]
                           : lcs[i][j - 1];
            }
        }
    }
    
    typedef enum { LINE_SAME, LINE_ADD, LINE_DEL } LineType;
    typedef struct { LineType type; char text[MAX_LINE_LEN]; } DiffEntry;

    DiffEntry *entries = (DiffEntry *)malloc((old_count + new_count + 1) * sizeof(DiffEntry));
    int entry_count = 0;

    int i = old_count, j = new_count;
    while (i > 0 || j > 0) {
        if (i > 0 && j > 0 && strcmp(old_lines[i - 1], new_lines[j - 1]) == 0) {
            entries[entry_count].type = LINE_SAME;
            strncpy(entries[entry_count].text, old_lines[i - 1], MAX_LINE_LEN - 1);
            entry_count++;
            i--; j--;
        } else if (j > 0 && (i == 0 || lcs[i][j - 1] >= lcs[i - 1][j])) {
            entries[entry_count].type = LINE_ADD;
            strncpy(entries[entry_count].text, new_lines[j - 1], MAX_LINE_LEN - 1);
            entry_count++;
            j--;
        } else {
            entries[entry_count].type = LINE_DEL;
            strncpy(entries[entry_count].text, old_lines[i - 1], MAX_LINE_LEN - 1);
            entry_count++;
            i--;
        }
    }

    printf("\n  diff: %s\n", label);
    printf("  --- last commit\n");
    printf("  +++ working tree\n");
    printf("  ──────────────────────────────────────\n");

    int has_changes = 0;
    for (int k = entry_count - 1; k >= 0; k--) {
        if (entries[k].type == LINE_DEL) {
            printf("  - %s\n", entries[k].text);
            has_changes = 1;
        } else if (entries[k].type == LINE_ADD) {
            printf("  + %s\n", entries[k].text);
            has_changes = 1;
        }

    }

    if (!has_changes)
        printf("  (no changes)\n");

    printf("  ──────────────────────────────────────\n");

    for (int k = 0; k <= old_count; k++)
        free(lcs[k]);
    free(lcs);
    free(entries);
    free(old_lines);
    free(new_lines);
}

int cmd_diff(const char *filename) {
    if (!path_exists(CHRONICLE_DIR)) {
        fprintf(stderr, "Error: Not a Chronicle repository.\n");
        return 1;
    }

    int head = get_head();
    if (head == 0) {
        printf("No commits yet. Nothing to diff against.\n");
        return 0;
    }

    char files_path[256];
    snprintf(files_path, sizeof(files_path), "%s/%04d/files", COMMITS_DIR, head);

    if (filename) {
        char old_path[512];
        build_path(old_path, sizeof(old_path), files_path, filename);

        if (!path_exists(old_path)) {
            printf("'%s' is not tracked in the last commit.\n", filename);
            return 1;
        }
        if (!path_exists(filename)) {
            printf("File '%s' has been deleted from the working tree.\n", filename);
            return 0;
        }
        diff_files(old_path, filename, filename);
    } else {
        char meta_path[256];
        snprintf(meta_path, sizeof(meta_path), "%s/%04d/meta.txt", COMMITS_DIR, head);

        FILE *meta = fopen(meta_path, "r");
        if (!meta) {
            fprintf(stderr, "Error: Cannot read last commit metadata.\n");
            return 1;
        }

        char line[512];
        while (fgets(line, sizeof(line), meta)) {
            if (strncmp(line, "files=", 6) == 0) {
                line[strcspn(line, "\n")] = '\0';
                char *token = strtok(line + 6, ",");
                while (token) {
                    char old_path[512];
                    build_path(old_path, sizeof(old_path), files_path, token);

                    if (path_exists(token)) {
                        char old_hash[17], new_hash[17];
                        hash_file(old_path, old_hash);
                        hash_file(token, new_hash);
                        if (strcmp(old_hash, new_hash) != 0)
                            diff_files(old_path, token, token);
                    } else {
                        printf("\n  '%s' was deleted from the working tree.\n", token);
                    }
                    token = strtok(NULL, ",");
                }
            }
        }
        fclose(meta);
    }

    return 0;
}

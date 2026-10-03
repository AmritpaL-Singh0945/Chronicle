#include "log.h"
#include "utils.h"
#include <stdio.h>

int cmd_log(void) {
    if (!path_exists(CHRONICLE_DIR)) {
        fprintf(stderr, "Error: Not a Chronicle repository.\n");
        return 1;
    }

    int head = get_head();
    if (head == 0) {
        printf("No commits yet.\n");
        return 0;
    }

    printf("\n  Chronicle Log\n");
    printf("  ─────────────────────────────────────────\n");

    /* Print commits from newest to oldest */
    for (int id = head; id >= 1; id--) {
        char meta_path[256];
        snprintf(meta_path, sizeof(meta_path), "%s/%04d/meta.txt", COMMITS_DIR, id);

        FILE *meta = fopen(meta_path, "r");
        if (!meta) continue;

        char line[512];
        char msg[256]  = "";
        char time[64]  = "";
        char files[512] = "";

        while (fgets(line, sizeof(line), meta)) {
            if (strncmp(line, "message=", 8) == 0) {
                strncpy(msg,   line + 8, sizeof(msg)  - 1);
                msg[strcspn(msg,   "\n")] = '\0';
            } else if (strncmp(line, "timestamp=", 10) == 0) {
                strncpy(time,  line + 10, sizeof(time) - 1);
                time[strcspn(time, "\n")] = '\0';
            } else if (strncmp(line, "files=", 6) == 0) {
                strncpy(files, line + 6,  sizeof(files) - 1);
                files[strcspn(files, "\n")] = '\0';
            }
        }
        fclose(meta);

        printf("  commit %04d\n", id);
        printf("  Date:    %s\n", time);
        printf("  Message: %s\n", msg);
        printf("  Files:   %s\n", files);
        printf("  ─────────────────────────────────────────\n");
    }

    return 0;
}

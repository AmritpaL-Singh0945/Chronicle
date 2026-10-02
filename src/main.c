#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "init.h"
#include "add.h"
#include "commit.h"
#include "log.h"
#include "diff.h"
#include "status.h"
#include "rollback.h"
#include "guard.h"
#include "utils.h"

static void print_usage(void) {
    printf("\nChronicle — A lightweight version control system\n\n");
    printf("Usage:  chronicle <command> [options]\n\n");
    printf("Commands:\n");
    printf("  init                    Initialize a new repository\n");
    printf("  add <file>              Stage a file for commit\n");
    printf("  commit -m <message>     Save a snapshot with a message\n");
    printf("  log                     View commit history\n");
    printf("  diff [file]             Show changes vs last commit\n");
    printf("  status                  Show staged and unstaged changes\n");
    printf("  rollback <id>           Revert working tree to a past commit\n");
    printf("  guard <file>            Mark a file as guarded (Chronicle Guard)\n\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage();
        return 1;
    }

    char *cmd = argv[1];

    if (strcmp(cmd, "init") == 0) {
        return cmd_init();

    } else if (strcmp(cmd, "add") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Usage: chronicle add <file>\n");
            return 1;
        }
        return cmd_add(argv[2]);

    } else if (strcmp(cmd, "commit") == 0) {
        if (argc < 4 || strcmp(argv[2], "-m") != 0) {
            fprintf(stderr, "Usage: chronicle commit -m <message>\n");
            return 1;
        }
        return cmd_commit(argv[3]);

    } else if (strcmp(cmd, "log") == 0) {
        return cmd_log();

    } else if (strcmp(cmd, "diff") == 0) {
        const char *file = (argc >= 3) ? argv[2] : NULL;
        return cmd_diff(file);

    } else if (strcmp(cmd, "status") == 0) {
        return cmd_status();

    } else if (strcmp(cmd, "rollback") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Usage: chronicle rollback <commit_id>\n");
            return 1;
        }
        return cmd_rollback(atoi(argv[2]));

    } else if (strcmp(cmd, "guard") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Usage: chronicle guard <file>\n");
            return 1;
        }
        return cmd_guard(argv[2]);

    } else {
        fprintf(stderr, "Unknown command: '%s'\n", cmd);
        print_usage();
        return 1;
    }
}

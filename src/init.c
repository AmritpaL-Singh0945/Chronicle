#include "init.h"
#include "utils.h"
#include <stdio.h>

int cmd_init(void) {
    if (path_exists(CHRONICLE_DIR)) {
        printf("Chronicle repository already exists in this directory.\n");
        return 0;
    }

    if (make_dir(CHRONICLE_DIR) != 0 ||
        make_dir(COMMITS_DIR)   != 0 ||
        make_dir(STAGING_DIR)   != 0) {
        fprintf(stderr, "Error: Could not create repository structure.\n");
        return 1;
    }

    /* Start with commit count 0 (no commits yet) */
    set_head(0);

    write_file(INDEX_FILE,  "");
    write_file(GUARDS_FILE, "");

    printf("Initialized empty Chronicle repository in .chronicle/\n");
    return 0;
}

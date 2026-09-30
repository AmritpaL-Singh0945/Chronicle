#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define MAKE_DIR(p) mkdir(p, 0755)

/* Chronicle internal directory/file paths */
#define CHRONICLE_DIR   ".chronicle"
#define COMMITS_DIR     ".chronicle/commits"
#define STAGING_DIR     ".chronicle/staging"
#define HEAD_FILE       ".chronicle/HEAD"
#define INDEX_FILE      ".chronicle/staging_index.txt"
#define GUARDS_FILE     ".chronicle/guards.cfg"


int path_exists(const char *path);

int make_dir(const char *path);

void make_dirs(const char *path);

int copy_file(const char *src, const char *dest);

int copy_file_with_dirs(const char *src, const char *dest);

int get_head(void);

void set_head(int commit_id);

void build_path(char *out, size_t size, const char *prefix, const char *suffix);

char *read_file(const char *path);

int write_file(const char *path, const char *content);

void get_timestamp(char *buf, size_t size);

#endif

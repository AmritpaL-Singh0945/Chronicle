#include "utils.h"
#include <time.h>

int path_exists(const char *path) {
    struct stat st;
    return stat(path, &st) == 0;
}

int is_directory(const char *path) {
    struct stat st;
    if (stat(path, &st) != 0) return 0;
    return S_ISDIR(st.st_mode);
}

int make_dir(const char *path) {
    return MAKE_DIR(path);
}

void make_dirs(const char *path) {
    char tmp[512];
    strncpy(tmp, path, sizeof(tmp) - 1);
    tmp[sizeof(tmp) - 1] = '\0';

    for (int i = 1; tmp[i]; i++) {
        if (tmp[i] == '/') {
            tmp[i] = '\0';
            MAKE_DIR(tmp);      
            tmp[i] = '/';
        }
    }
    MAKE_DIR(tmp);
}

int copy_file(const char *src, const char *dest) {
    FILE *in = fopen(src, "rb");
    if (!in) return -1;

    FILE *out = fopen(dest, "wb");
    if (!out) { fclose(in); return -1; }

    char buf[4096];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), in)) > 0) {
        fwrite(buf, 1, n, out);
    }

    fclose(in);
    fclose(out);
    return 0;
}

int copy_file_with_dirs(const char *src, const char *dest) {
    char parent[512];
    strncpy(parent, dest, sizeof(parent) - 1);
    parent[sizeof(parent) - 1] = '\0';

    char *last_slash = strrchr(parent, '/');
    if (last_slash) {
        *last_slash = '\0';
        make_dirs(parent);
    }
    return copy_file(src, dest);
}

int get_head(void) {
    FILE *f = fopen(HEAD_FILE, "r");
    if (!f) return 0;
    int id = 0;
    fscanf(f, "%d", &id);
    fclose(f);
    return id;
}

void set_head(int commit_id) {
    FILE *f = fopen(HEAD_FILE, "w");
    if (!f) return;
    fprintf(f, "%d", commit_id);
    fclose(f);
}

void build_path(char *out, size_t size, const char *prefix, const char *suffix) {
    snprintf(out, size, "%s/%s", prefix, suffix);
}

char *read_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    char *buf = (char *)malloc(size + 1);
    if (!buf) { fclose(f); return NULL; }

    size_t read = fread(buf, 1, size, f);
    buf[read] = '\0';
    fclose(f);
    return buf;
}

int write_file(const char *path, const char *content) {
    FILE *f = fopen(path, "w");
    if (!f) return -1;
    fputs(content, f);
    fclose(f);
    return 0;
}

void get_timestamp(char *buf, size_t size) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buf, size, "%Y-%m-%d %H:%M:%S", tm_info);
}

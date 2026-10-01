#include "hash.h"
#include <stdio.h>

void hash_file(const char *filepath, char *out) {
    FILE *f = fopen(filepath, "rb");
    if (!f) {
        snprintf(out, 17, "0000000000000000");
        return;
    }

    unsigned long long hash = 14695981039346656037ULL;  
    int c;
    while ((c = fgetc(f)) != EOF) {
        hash ^= (unsigned char)c;
        hash *= 1099511628211ULL;                      
    }
    fclose(f);

    snprintf(out, 17, "%016llx", (unsigned long long)hash);
}

unsigned long long hash_string(const char *str) {
    unsigned long long hash = 14695981039346656037ULL;
    while (*str) {
        hash ^= (unsigned char)(*str++);
        hash *= 1099511628211ULL;
    }
    return hash;
}

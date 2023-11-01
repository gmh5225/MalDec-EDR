#pragma once

#include <uthash.h>

struct dir_list {
    char *dir;
    UT_hash_handle hh;
};

void add_ignore_dir(struct dir_list **ignore_list, const char *path);
void add_ignore_dirs_from_list(struct dir_list **ignore_list, const char *path[], size_t n);

struct dir_list* find_ignored(struct dir_list **ignore_list, const char *path);
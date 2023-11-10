#pragma once

#include <uthash.h>

struct ignored_dirs_list {
    char *dir;
    UT_hash_handle hh;
};

void add_ignore_dir(struct ignored_dirs_list **ignore_list, const char *path);
void add_ignore_dirs_from_list(struct ignored_dirs_list **ignore_list, const char *path[], size_t n);

struct ignored_dirs_list* find_ignored(struct ignored_dirs_list **ignore_list, const char *path);
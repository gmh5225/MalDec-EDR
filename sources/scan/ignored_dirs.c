#include <scan/ignored_dirs.h>

inline void add_ignore_dir(struct ignored_dirs_list **ignore_list, const char *path)
{
    struct ignored_dirs_list *ig = malloc(sizeof(struct ignored_dirs_list));
    ig->dir = strdup(path);
    HASH_ADD_STR(*ignore_list, dir, ig);
}

void add_ignore_dirs_from_list(struct ignored_dirs_list **ignore_list, const char *path[], size_t n)
{
    for (int i = 0; i < n; i++) {
        add_ignore_dir(ignore_list, path[i]);
    }
}

inline struct ignored_dirs_list* find_ignored(struct ignored_dirs_list **ignore_list, const char *path)
{
    struct ignored_dirs_list *ig;
    HASH_FIND_STR(*ignore_list, path, ig);
    return ig;
}
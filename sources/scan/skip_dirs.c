#include "skip_dirs.h"

#include "err/ptr_err.h"

int prefix(const char *pre, const char *str)
{
    return strncmp(pre, str, strlen(pre)) == 0;
}

void add_skip_dirs(struct skip_dirs **skip, const char *path[], size_t n)
{
    for (int i = 0; i < n; i++)
    {
        struct skip_dirs *skp = malloc(sizeof(struct skip_dirs));
        skp->dir = strdup(path[i]);
        HASH_ADD_STR(*skip, dir, skp);
    }
}

void del_skip_dirs(struct skip_dirs **skip)
{
    struct skip_dirs *current_user, *tmp;

    HASH_ITER(hh, *skip, current_user, tmp)
    {
        HASH_DEL(*skip, current_user); /* delete; users advances to next */
        free(current_user->dir);
        free(current_user); /* optional- if you want to free  */
    }
}

inline struct skip_dirs *get_skipped(struct skip_dirs *skip, const char *path)
{
    if (!skip)
        return NULL;

    struct skip_dirs *ig;
    HASH_FIND_STR(skip, path, ig);
    return ig;
}
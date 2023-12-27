#include "skip_dirs.h"

#include "err/err.h"

inline int
prefix(const char *pre, const char *str)
{
  return strncmp(pre, str, strlen(pre)) == 0;
}

inline void
add_skip_dirs(struct SKIP_DIRS **skip, const char *path[], size_t n)
{
  for (int i = 0; i < n; i++)
  {
    struct SKIP_DIRS *skp = malloc(sizeof(struct SKIP_DIRS));
    skp->dir              = strdup(path[i]);
    HASH_ADD_STR(*skip, dir, skp);
  }
}

inline void
del_skip_dirs(struct SKIP_DIRS **skip)
{
  struct SKIP_DIRS *current_user, *tmp;

  HASH_ITER(hh, *skip, current_user, tmp)
  {
    HASH_DEL(*skip, current_user); /* delete; users advances to next */
    free(current_user->dir);
    free(current_user);
  }
}

inline struct SKIP_DIRS *
get_skipped(struct SKIP_DIRS **skip, const char *path)
{
  if (!*skip) return NULL;

  struct SKIP_DIRS *ig;
  HASH_FIND_STR(*skip, path, ig);
  return ig;
}
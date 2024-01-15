#include "inotify/paths.h"

inline int
prefix(const char *pre, const char *str)
{
  return strncmp(pre, str, strlen(pre)) == 0;
}

inline void
add_paths(struct PATHS **paths, const char *path[], size_t n)
{
  for (size_t i = 0; i < n; i++)
  {
    struct PATHS *_path = malloc(sizeof(struct PATHS));
    _path->path          = strdup(path[i]);
    HASH_ADD_STR(*paths, path, _path);
  }
}

inline void
del_paths(struct PATHS **paths)
{
  struct PATHS *current_user, *tmp;

  HASH_ITER(hh, *paths, current_user, tmp)
  {
    HASH_DEL(*paths, current_user); /* delete; users advances to next */
    free(current_user->path);
    free(current_user);
  }
}
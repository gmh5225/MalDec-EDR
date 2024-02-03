#include "defender.h"
#include "inotify/inotify.h"

void
init_inotify_main(DEFENDER **defender)
{
  struct json_object *inotify_obj, *paths_obj;
  if (json_object_object_get_ex((*defender)->cjson, "inotify",
                                &inotify_obj))
  {
    if (!json_object_object_get_ex(inotify_obj, "paths", &paths_obj))
    {
      fprintf(stderr, LOG_MESSAGE_FORMAT("Unable to retrieve scan "
                                         "configuration from JSON\n"));
      exit(ERR_FAILURE);
    }
  }

  const int    length = json_object_array_length(paths_obj);
  const char **path   = alloca(length * sizeof(const char *));
  for (int i = 0; i < length; ++i)
  {
    struct json_object *element = json_object_array_get_idx(paths_obj, i);
    path[i] = (element != NULL) ? json_object_get_string(element) : NULL;
  }

  struct PATHS *paths = NULL;
  add_paths(&paths, path, length);

  INOTIFY_CONFIG config =
          (INOTIFY_CONFIG){.paths = paths, .quantity_fds = length};

  if (IS_ERR_FAILURE(init_inotify(&(*defender)->inotify, config)))
  {
    fprintf(stderr, LOG_MESSAGE_FORMAT("Error init inotify\n"));
    exit(ERR_FAILURE);
  }
}
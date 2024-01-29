#include "defender.h"

void
init_defender(DEFENDER **defender, DEFENDER_CONFIG config)
{
  *defender                = malloc(sizeof(struct DEFENDER));
  (*defender)->config      = config;
  (*defender)->inotify     = NULL;
  (*defender)->scanner     = NULL;
  (*defender)->telekinesis = NULL;
  (*defender)->config_json = NULL;
  (*defender)->logger      = NULL;

  ALLOC_ERR_FAILURE(*defender);
}

void
init_cjson_main(DEFENDER **defender)
{
  if (IS_ERR_FAILURE(init_json(&(*defender)->config_json,
                               (*defender)->config.settings_json_path)))
  {
    fprintf(stderr, LOG_MESSAGE_FORMAT("Error in parser json config '%s'\n",
                                       (*defender)->config.settings_json_path));
    exit(EXIT_FAILURE);
  }
}

void
init_inotify_main(DEFENDER **defender)
{
  struct json_object *inotify_obj, *paths_obj;
  if (json_object_object_get_ex((*defender)->config_json, "inotify",
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

void
init_logger_main(DEFENDER **defender)
{
  struct json_object *logger_obj, *filename_obj, *max_file_size_obj,
          *max_backup_files_obj, *level_obj, *console_obj;

  if (json_object_object_get_ex((*defender)->config_json, "logger",
                                &logger_obj))
  {
    if (!json_object_object_get_ex(logger_obj, "filename", &filename_obj) ||
        !json_object_object_get_ex(logger_obj, "max_file_size",
                                   &max_file_size_obj) ||
        !json_object_object_get_ex(logger_obj, "max_backup_files",
                                   &max_backup_files_obj) ||
        !json_object_object_get_ex(logger_obj, "level", &level_obj) ||
        !json_object_object_get_ex(logger_obj, "console", &console_obj))
    {
      fprintf(stderr, LOG_MESSAGE_FORMAT("Unable to retrieve logger "
                                         "configuration from JSON\n"));
      exit(ERR_FAILURE);
    }
  }
  else
  {
    fprintf(stderr, LOG_MESSAGE_FORMAT("Unable to retrieve 'logger' object "
                                       "from JSON\n"));
    exit(ERR_FAILURE);
  }

  LOGGER_CONFIG logger_config = (LOGGER_CONFIG){
          .filename         = json_object_get_string(filename_obj),
          .level            = json_object_get_int(level_obj),
          .max_backup_files = json_object_get_int(max_backup_files_obj),
          .max_file_size    = json_object_get_int(max_file_size_obj),
          .console          = json_object_get_boolean(console_obj)};

  if (IS_ERR_FAILURE(init_logger(&(*defender)->logger, logger_config)))
  {
    fprintf(stderr, LOG_MESSAGE_FORMAT("Error init logger\n"));
    exit(ERR_FAILURE);
  }
}

void
init_telekinesis_main(DEFENDER **defender)
{
  struct json_object *telekinesis_obj, *driver_path_obj, *driver_name_obj;

  if (json_object_object_get_ex((*defender)->config_json, "driver_telekinesis",
                                &telekinesis_obj))
  {
    if (!json_object_object_get_ex(telekinesis_obj, "driver_path",
                                   &driver_path_obj) ||
        !json_object_object_get_ex(telekinesis_obj, "driver_name",
                                   &driver_name_obj))
    {
      fprintf(stderr, LOG_MESSAGE_FORMAT("Unable to retrieve "
                                         "driver_telekinesis "
                                         "configuration from JSON\n"));
      exit(ERR_FAILURE);
    }
  }

  TELEKINESIS_CONFIG config = (TELEKINESIS_CONFIG){
          .driver_name = json_object_get_string(driver_name_obj),
          .driver_path = json_object_get_string(driver_path_obj)};

  if (IS_ERR_FAILURE(
              init_driver_telekinesis(&(*defender)->telekinesis, config)))
  {
    fprintf(stderr, LOG_MESSAGE_FORMAT("Error in init driver %s\n",
                                       config.driver_name));
    exit(EXIT_FAILURE);
  }
}

void
init_scanner_main(DEFENDER **defender)
{
  struct json_object *scan_obj, *yara_obj, *rules_obj, *skip_dir_objs;
  if (json_object_object_get_ex((*defender)->config_json, "scanner", &scan_obj))
  {
    if (!json_object_object_get_ex(scan_obj, "yara", &yara_obj) ||
        !json_object_object_get_ex(scan_obj, "skip_dirs", &skip_dir_objs) ||
        !json_object_object_get_ex(yara_obj, "rules", &rules_obj))
    {
      fprintf(stderr, LOG_MESSAGE_FORMAT("Unable to retrieve scan "
                                         "configuration from JSON\n"));
      exit(ERR_FAILURE);
    }
  }

  const int    length   = json_object_array_length(skip_dir_objs);
  const char **skip_dir = alloca(length * sizeof(const char *));
  for (int i = 0; i < length; ++i)
  {
    struct json_object *element = json_object_array_get_idx(skip_dir_objs, i);
    skip_dir[i] = (element != NULL) ? json_object_get_string(element) : NULL;
  }

  struct SKIP_DIRS *skip = NULL;
  add_skip_dirs(&skip, skip_dir, length);

  SCAN_CONFIG config = (SCAN_CONFIG){.file_path = NULL,
                                     .max_depth = -1,
                                     .scan_type = 0,
                                     .verbose   = false,
                                     .inotify   = NULL,
                                     .skip_dirs = skip};
  config.yara.rules  = json_object_get_string(rules_obj);

  if (IS_ERR_FAILURE(init_scanner(&(*defender)->scanner, config)))
  {
    fprintf(stderr, LOG_MESSAGE_FORMAT("Error init scanner\n"));
    exit(ERR_FAILURE);
  }
}

void
exit_defender(DEFENDER **defender)
{
  free((*defender));
  NO_USE_AFTER_FREE((*defender));
}
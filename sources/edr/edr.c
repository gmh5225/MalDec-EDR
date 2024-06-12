#include "edr.h"

inline void
init_edr(EDR **edr, EDR_CONFIG config)
{
  *edr = malloc(sizeof(struct EDR));
  ALLOC_ERR_FAILURE(*edr);

  (*edr)->config    = config;
  (*edr)->inotify   = NULL;
  (*edr)->scanner   = NULL;
  (*edr)->cjson     = NULL;
  (*edr)->logger    = NULL;
  (*edr)->inspector = NULL;
  (*edr)->crowarmor = NULL;
}

inline void
init_inspector_main(EDR **edr)
{
  struct json_object *inspector_obj, *dir_obj, *database_obj, *quarantine_obj,
          *quarantine_dir_obj;

  if (json_object_object_get_ex((*edr)->cjson, "inspector", &inspector_obj))
  {
    if (!json_object_object_get_ex(inspector_obj, "dir", &dir_obj) ||
        !json_object_object_get_ex(inspector_obj, "database", &database_obj) ||
        !json_object_object_get_ex(inspector_obj, "quarantine",
                                   &quarantine_obj) ||
        !json_object_object_get_ex(quarantine_obj, "dir", &quarantine_dir_obj))
    {
      fprintf(stderr, LOG_MESSAGE_FORMAT("Unable to retrieve scan "
                                         "configuration from JSON\n"));
      exit(ERR_FAILURE);
    }
  }

  INSPECTOR_CONFIG config = (INSPECTOR_CONFIG){
          .dir            = json_object_get_string(dir_obj),
          .quarantine.dir = json_object_get_string(quarantine_dir_obj),
          .database       = json_object_get_string(database_obj)};

#ifdef DEBUG
  LOG_DEBUG(LOG_MESSAGE_FORMAT("inspector.config.dir = '%s', "
                               "inspector.config.quarantine.dir = '%s' "
                               "inspector.config.database = '%s'",
                               config.dir, config.quarantine.dir,
                               config.database));

#endif

  if (IS_ERR_FAILURE(init_inspector(&(*edr)->inspector, config)))
  {
    fprintf(stderr, LOG_MESSAGE_FORMAT("Error init inspector\n"));
    exit(ERR_FAILURE);
  }
}

inline void
init_cjson_main(EDR **edr)
{
  if (IS_ERR_FAILURE(
              init_cjson(&(*edr)->cjson, (*edr)->config.settings_json_path)))
  {
    fprintf(stderr, LOG_MESSAGE_FORMAT("Error in parser json config '%s'\n",
                                       (*edr)->config.settings_json_path));
    exit(EXIT_FAILURE);
  }
}

inline void
init_inotify_main(EDR **edr)
{
  struct json_object *inotify_obj, *paths_obj;
  if (json_object_object_get_ex((*edr)->cjson, "inotify", &inotify_obj))
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

#ifdef DEBUG
  LOG_DEBUG(LOG_MESSAGE_FORMAT("inotify.config.quantity_fds = '%i'",
                               config.quantity_fds));

#endif

  if (IS_ERR_FAILURE(init_inotify(&(*edr)->inotify, config)))
  {
    fprintf(stderr, LOG_MESSAGE_FORMAT("Error init inotify\n"));
    exit(ERR_FAILURE);
  }
}

inline void
init_logger_main(EDR **edr)
{
  struct json_object *logger_obj, *filename_obj, *max_file_size_obj,
          *max_backup_files_obj, *level_obj, *console_obj;

  if (json_object_object_get_ex((*edr)->cjson, "logger", &logger_obj))
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

  LOGGER_CONFIG config = (LOGGER_CONFIG){
          .filename         = json_object_get_string(filename_obj),
          .level            = json_object_get_int(level_obj),
          .max_backup_files = json_object_get_int(max_backup_files_obj),
          .max_file_size    = json_object_get_int(max_file_size_obj),
          .console          = json_object_get_boolean(console_obj)};

  if (IS_ERR_FAILURE(init_logger(&(*edr)->logger, config)))
  {
    fprintf(stderr, LOG_MESSAGE_FORMAT("Error init logger\n"));
    exit(ERR_FAILURE);
  }

#ifdef DEBUG
  LOG_DEBUG(LOG_MESSAGE_FORMAT("logger.config.filename = '%s', "
                               "logger.config.level = '%i', "
                               "logger.max_backup_files = '%i', "
                               "logger.max_file_size = '%i', "
                               "logger.config.console = '%i'",
                               config.filename, config.level,
                               config.max_backup_files, config.max_file_size,
                               config.console));

#endif
}

void
init_crowarmor_main(EDR **edr)
{
  struct json_object *crowarmor_obj, *driver_path_obj, *driver_name_obj;

  if (json_object_object_get_ex((*edr)->cjson, "driver_crowarmor",
                                &crowarmor_obj))
  {
    if (!json_object_object_get_ex(crowarmor_obj, "driver_path",
                                   &driver_path_obj) ||
        !json_object_object_get_ex(crowarmor_obj, "driver_name",
                                   &driver_name_obj))
    {
      fprintf(stderr, LOG_MESSAGE_FORMAT("Unable to retrieve "
                                         "driver_crowarmor "
                                         "configuration from JSON\n"));
      exit(ERR_FAILURE);
    }
  }

  CROWARMOR_CONFIG config = (CROWARMOR_CONFIG){
          .driver_name = json_object_get_string(driver_name_obj),
          .driver_path = json_object_get_string(driver_path_obj)};

#ifdef DEBUG
  LOG_DEBUG(LOG_MESSAGE_FORMAT("crowarmor.config.driver_name = '%s', "
                               "crowarmor.config.driver_path = '%s' ",
                               config.driver_name, config.driver_path));

#endif

  if (IS_ERR_FAILURE(init_driver_crowarmor(&(*edr)->crowarmor, config)))
  {
    fprintf(stderr, LOG_MESSAGE_FORMAT("Error in init driver %s\n",
                                       config.driver_name));
    exit(EXIT_FAILURE);
  }
}

inline void
init_scanner_main(EDR **edr)
{
  struct json_object *scan_obj, *yara_obj, *rules_obj, *skip_dir_objs;
  if (json_object_object_get_ex((*edr)->cjson, "scanner", &scan_obj))
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

  SCANNER_CONFIG config =
          (SCANNER_CONFIG){.filepath   = NULL,
                           .max_depth  = -1,
                           .scan_type  = 0,
                           .verbose    = false,
                           .inotify    = NULL,
                           .skip_dirs  = skip,
                           .yara.rules = json_object_get_string(rules_obj)};

#ifdef DEBUG
  LOG_DEBUG(LOG_MESSAGE_FORMAT("scanner.config.filepath = '%s', "
                               "scanner.config.max_depth = '%i', "
                               "scanner.config.verbose = '%i', "
                               "scanner.config.yara.rules = '%s', "
                               "scanner.config.scan_type = '%i'",
                               config.filepath, config.max_depth,
                               config.verbose, config.yara.rules,
                               config.scan_type));

#endif

  if (IS_ERR_FAILURE(init_scanner(&(*edr)->scanner, config)))
  {
    fprintf(stderr, LOG_MESSAGE_FORMAT("Error init scanner\n"));
    exit(ERR_FAILURE);
  }
}

inline void
exit_edr(EDR **edr)
{
  free((*edr));
  NO_USE_AFTER_FREE((*edr));
}
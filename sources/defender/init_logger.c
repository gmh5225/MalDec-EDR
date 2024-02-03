#include "defender.h"
#include "err/err.h"
#include "logger/logger.h"

void
init_logger_main(DEFENDER **defender)
{
  struct json_object *logger_obj, *filename_obj, *max_file_size_obj,
          *max_backup_files_obj, *level_obj, *console_obj;

  if (json_object_object_get_ex((*defender)->cjson, "logger",
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
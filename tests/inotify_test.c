#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include "cmocka.h"
#include "err/err.h"
#include "inotify/inotify.h"
#include "logger/logger.h"

static void
inotify(void **state)
{
  INOTIFY *inotify;

  const int    length = 1;
  const char **path   = alloca(length * sizeof(const char *));
  for (int i = 0; i < length; ++i)
  {
    path[i] = "./";
  }

  struct PATHS *paths = NULL;
  add_paths(&paths, path, length);
  INOTIFY_CONFIG config =
          (INOTIFY_CONFIG){.paths = paths, .quantity_fds = 1, .time = 1};

  assert_int_equal(init_inotify(&inotify, config), ERR_SUCCESS);
  exit_inotify(&inotify);
  (void)state;
}

int
main(void)
{
  LOGGER_CONFIG logger_config = (LOGGER_CONFIG){.filename = "testinotify.log",
                                                .level    = 0,
                                                .max_backup_files = 0,
                                                .max_file_size    = 0};
  LOGGER       *logger;

  if (IS_ERR_FAILURE(init_logger(&logger, logger_config)))
  {
    fprintf(stderr, LOG_MESSAGE_FORMAT("Error during logger initialization. "
                                       "Please review the 'appsettings.json' "
                                       "file and ensure that the 'logger' "
                                       "configuration is accurate."));
    exit(EXIT_FAILURE);
  }

  const struct CMUnitTest tests[] = {
          cmocka_unit_test(inotify),
  };

  exit_logger(&logger);
  
  return cmocka_run_group_tests(tests, NULL, NULL);
}
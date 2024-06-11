#include <fcntl.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <time.h>

#include "cmocka.h"
#include "err/err.h"
#include "inspector/inspector.h"
#include "logger/logger.h"

static void
inspector_quarantine_add(void **state)
{
  INSPECTOR       *inspector;
  INSPECTOR_CONFIG config = (INSPECTOR_CONFIG){.quarantine.dir = ".quarantine",
                                               .dir            = "inspector/",
                                               .database = "database.sqlite3"};
  if (!IS_ERR_FAILURE(init_inspector(&inspector, config)))
  {
    time_t           datetime = time(NULL);
    QUARANTINE_FILE file     = (QUARANTINE_FILE){.filepath = "./malware.elf",
                                                   .detected = "elf_infected",
                                                   .filename = "malware.elf",
                                                   .datetime = ctime(&datetime)};
    assert_int_equal(add_quarantine_inspector(inspector, &file), ERR_SUCCESS);
    exit_inspector(&inspector);
  }

  (void)state;
}

static void
inspector_quarantine_del(void **state)
{
  INSPECTOR       *inspector;
  INSPECTOR_CONFIG config = (INSPECTOR_CONFIG){.quarantine.dir = ".quarantine",
                                               .dir            = "inspector/",
                                               .database = "database.sqlite3"};
  if (!IS_ERR_FAILURE(init_inspector(&inspector, config)))
  {
    time_t           datetime = time(NULL);
    QUARANTINE_FILE file     = (QUARANTINE_FILE){.id = 1};

    assert_int_equal(del_quarantine_inspector(inspector, &file), ERR_SUCCESS);
    exit_inspector(&inspector);
    assert_int_equal(unlink("inspector/database.sqlite3"), 0);
  }
  
  (void)state;
}

int
main(void)
{
  LOGGER_CONFIG logger_config = (LOGGER_CONFIG){.filename = "testinspector.log",
                                                .level    = 0,
                                                .max_backup_files = 0,
                                                .max_file_size    = 0};
  LOGGER       *logger;
  creat("malware.elf", 0644);

  if (IS_ERR_FAILURE(init_logger(&logger, logger_config)))
  {
    fprintf(stderr, LOG_MESSAGE_FORMAT("Error during logger initialization. "
                                       "Please review the 'appsettings.json' "
                                       "file and ensure that the 'logger' "
                                       "configuration is accurate."));
    exit(EXIT_FAILURE);
  }
  const struct CMUnitTest tests[] = {
          cmocka_unit_test(inspector_quarantine_add),
          cmocka_unit_test(inspector_quarantine_del),
  };

  int ret = cmocka_run_group_tests(tests, NULL, NULL);
  exit_logger(&logger);

  return ret;
}
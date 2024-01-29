#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include "cmocka.h"

#include "err/err.h"
#include "logger/logger.h"
#include "scanner/scanner.h"

static void
yara_scanner(void **state)
{
  SCANNER *scanner;

  SCAN_CONFIG config = (SCAN_CONFIG){.file_path = "./",
                                     .max_depth = -1,
                                     .scan_type = 0,
                                     .skip_dirs = NULL};
  config.yara.rules  = "../rules/"
                       "YARA-Mindshield-Analysis";

  assert_int_equal(init_scanner(&scanner, config), ERR_SUCCESS);
  assert_int_equal(exit_scanner(&scanner), ERR_SUCCESS);

  (void)state;
}

int
main(void)
{
  LOGGER_CONFIG logger_config = (LOGGER_CONFIG){.filename = "testeyara.log",
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
          cmocka_unit_test(yara_scanner),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
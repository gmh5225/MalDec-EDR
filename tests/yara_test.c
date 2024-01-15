#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include "cmocka.h"
#include "err/err.h"
#include "logger/logger.h"
#include "scan/scan.h"

static void
yara_scan(void **state)
{
  SCANNER *scanner;

  SCANNER_CONFIG config = (SCANNER_CONFIG){.file_path = "./",
                                           .max_depth = -1,
                                           .scan_type = 0,
                                           .skip      = NULL,
                                           .rules     = "../rules/"
                                                        "YARA-Mindshield-Analysis"};

  if (!IS_ERR_FAILURE(init_scanner(&scanner, config)))
  {
    assert_int_equal(scan_dir(scanner, DEFAULT_SCAN_CALLBACK, 0), ERR_SUCCESS);
    assert_int_equal(exit_scanner(&scanner), ERR_SUCCESS);
    (void)state;
  }
}

static void
yara_scan_ignored(void **state)
{
  SCANNER *scanner;

  SCANNER_CONFIG config = (SCANNER_CONFIG){.file_path = "./",
                                           .max_depth = -1,
                                           .scan_type = 0,
                                           .rules     = "../rules/"
                                                        "YARA-Mindshield-Analysis"};

  if (!IS_ERR_FAILURE(init_scanner(&scanner, config)))
  {
    struct SKIP_DIRS *skip        = NULL;
    const char       *skip_list[] = {"/tmp/test"};
    add_skip_dirs(&skip, skip_list, 1);

    scanner->config.skip = skip;

    assert_int_equal(scan_dir(scanner, DEFAULT_SCAN_CALLBACK, 0), ERR_SUCCESS);
    assert_int_equal(exit_scanner(&scanner), ERR_SUCCESS);

    (void)state;
  }
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
          cmocka_unit_test(yara_scan),
          cmocka_unit_test(yara_scan_ignored),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
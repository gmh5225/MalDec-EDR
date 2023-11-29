#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include <scan/scan.h>
#include "err/err.h"
#include "logger/logger.h"

static void yara_scanner(void **state)
{
    SCANNER *scanner;

    SCANNER_CONFIG config = (SCANNER_CONFIG){
        .file_path = "/tmp",
        .max_depth = -1,
        .scan_type = 0,
        .skip = NULL,
    };

    assert_int_equal(init_scanner(&scanner, config), 0);
    assert_int_equal(exit_scanner(&scanner), 0);

    (void)state;
}

static void yara_scan(void **state)
{
    SCANNER *scanner;

    SCANNER_CONFIG config = (SCANNER_CONFIG){
        .file_path = "/tmp",
        .max_depth = -1,
        .scan_type = 0,
        .skip = NULL,
    };

    if (init_scanner(&scanner, config) != ERROR)
    {

        assert_int_equal(scan_dir(scanner, DEFAULT_SCAN_CALLBACK, 0), 0);
        assert_int_equal(exit_scanner(&scanner), 0);

        (void)state;
    }
}

static void yara_scan_ignored(void **state)
{
    SCANNER *scanner;

    SCANNER_CONFIG config = (SCANNER_CONFIG){
        .file_path = "/tmp",
        .max_depth = -1,
        .scan_type = 0,
    };

    if (init_scanner(&scanner, config) != ERROR)
    {
        struct skip_dirs *skip = NULL;
        const char *skip_list[] = {"/tmp/test"};
        add_skip_dirs(&skip, skip_list, 1);

        scanner->config.skip = skip;

        assert_int_equal(scan_dir(scanner, DEFAULT_SCAN_CALLBACK, 0), 0);
        assert_int_equal(exit_scanner(&scanner), 0);


        (void)state;
    }
}

int main(void)
{
    init_logger("testlog.txt");
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(yara_scanner),
        cmocka_unit_test(yara_scan),
        cmocka_unit_test(yara_scan_ignored),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
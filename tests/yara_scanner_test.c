#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "scan/scan.h"
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

int main(void)
{
    init_logger("testlog.txt");
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(yara_scanner),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include <scan/scan.h>

static void yara_scanner(void **state) {
    SCANNER *scanner;
    
    assert_int_equal(scanner_init(&scanner), 0);

    (void)state;
}

static void yara_scan(void **state) {
    SCANNER *scanner;
    scanner_init(&scanner);

    assert_int_equal(scan_folder(scanner, "/", DEFAULT_SCAN_CALLBACK), 0);

    (void)state;
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(yara_scanner),
        cmocka_unit_test(yara_scan),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
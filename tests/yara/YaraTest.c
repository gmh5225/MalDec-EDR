#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include <yara/yr_inspector.h>

static void yara_inspector(void **state) {
    INSPECTOR *inspector;
    
    assert_int_equal(inspector_init(&inspector), 0);

    (void)state;
}

static void yara_inspector_scan_folder(void **state) {
    INSPECTOR *inspector;
    inspector_init(&inspector);

    assert_int_equal(inspector_scan_folder(inspector, "/", default_scan_callback), 0);

    (void)state;
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(yara_inspector),
        cmocka_unit_test(yara_inspector_scan_folder),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
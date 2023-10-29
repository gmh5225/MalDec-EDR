#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include <yr_inspector.h>

static void yara_inspector(void **state) {
    INSPECTOR *inspector;
    inspector_init(&inspector);

    (void)state;
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(yara_inspector),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
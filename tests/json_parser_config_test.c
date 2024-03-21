#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include "cmocka.h"
#include "cjson/cjson.h"
#include "err/err.h"
#include "logger/logger.h"
#include "scanner/scanner.h"

#define CONFIG_JSON_PATH "../config/appsettings.json"

static void
json_config(void **state)
{
  struct json_object *json_obj;
  assert_int_equal(init_cjson(&json_obj, CONFIG_JSON_PATH), ERR_SUCCESS);

  struct json_object *logger_obj, *filename_obj, *max_file_size_obj,
          *max_backup_files_obj, *level_obj;

  assert_int_equal(json_object_object_get_ex(json_obj, "logger", &logger_obj),
                   true);
  assert_int_equal(json_object_object_get_ex(logger_obj, "filename",
                                             &filename_obj),
                   true);
  assert_int_equal(json_object_object_get_ex(logger_obj, "max_file_size",
                                             &max_file_size_obj),
                   true);
  assert_int_equal(json_object_object_get_ex(logger_obj, "max_backup_files",
                                             &max_backup_files_obj),
                   true);
  assert_int_equal(json_object_object_get_ex(logger_obj, "level", &level_obj),
                   true);

  exit_json(&json_obj);
  (void)state;
}

int
main(void)
{
  const struct CMUnitTest tests[] = {
          cmocka_unit_test(json_config),
  };

  int ret = cmocka_run_group_tests(tests, NULL, NULL);

  return ret;
}
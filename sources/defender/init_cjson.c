#include "cjson/cjson.h"
#include "defender.h"

void
init_cjson_main(DEFENDER **defender)
{
  if (IS_ERR_FAILURE(init_json(&(*defender)->cjson,
                               (*defender)->config.settings_json_path)))
  {
    fprintf(stderr, LOG_MESSAGE_FORMAT("Error in parser json config '%s'\n",
                                       (*defender)->config.settings_json_path));
    exit(EXIT_FAILURE);
  }
}
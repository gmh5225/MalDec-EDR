#include "defender.h"
#include "telekinesis/telekinesis.h"

void
init_telekinesis_main(DEFENDER **defender)
{
  struct json_object *telekinesis_obj, *driver_path_obj, *driver_name_obj;

  if (json_object_object_get_ex((*defender)->cjson, "driver_telekinesis",
                                &telekinesis_obj))
  {
    if (!json_object_object_get_ex(telekinesis_obj, "driver_path",
                                   &driver_path_obj) ||
        !json_object_object_get_ex(telekinesis_obj, "driver_name",
                                   &driver_name_obj))
    {
      fprintf(stderr, LOG_MESSAGE_FORMAT("Unable to retrieve "
                                         "driver_telekinesis "
                                         "configuration from JSON\n"));
      exit(ERR_FAILURE);
    }
  }

  TELEKINESIS_CONFIG config = (TELEKINESIS_CONFIG){
          .driver_name = json_object_get_string(driver_name_obj),
          .driver_path = json_object_get_string(driver_path_obj)};

  if (IS_ERR_FAILURE(
              init_driver_telekinesis(&(*defender)->telekinesis, config)))
  {
    fprintf(stderr, LOG_MESSAGE_FORMAT("Error in init driver %s\n",
                                       config.driver_name));
    exit(EXIT_FAILURE);
  }
}
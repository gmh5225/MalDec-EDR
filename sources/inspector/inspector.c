#include "inspector/inspector.h"

ERR
init_inspector(INSPECTOR **inspector, INSPECTOR_CONFIG config)
{
  *inspector = malloc(sizeof(struct INSPECTOR));
  ALLOC_ERR_FAILURE(*inspector);

  (*inspector)->config = config;

  return ERR_SUCCESS;
}

void
exit_inspector(INSPECTOR **inspector)
{
  free(*inspector);
  NO_USE_AFTER_FREE(*inspector);
}
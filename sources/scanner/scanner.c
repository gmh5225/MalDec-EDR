#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <yara.h>

#include "err/err.h"
#include "logger/logger.h"
#include "scanner.h"

inline static ERR
scanner_set_rule(SCANNER *scanner, const char *path, const char *yara_filename)
{
  ERR                retval   = ERR_SUCCESS;
  YR_FILE_DESCRIPTOR rules_fd = open(path, O_RDONLY);

  if (yr_compiler_add_fd(scanner->yr_compiler, rules_fd, NULL, yara_filename) !=
      ERROR_SUCCESS)
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE"));
    retval = ERR_FAILURE;
    goto ret;
  }

ret:
  close(rules_fd);
  return retval;
}

inline static ERR
scanner_load_rules(SCANNER *scanner, const char *dir)
{
  ERR            retval = ERR_SUCCESS;
  DIR           *dd;
  struct dirent *entry;
  const size_t   dir_size = strlen(dir);

  if (IS_NULL_PTR((dd = opendir(dir))))
  {
    LOG_ERROR(
            LOG_MESSAGE_FORMAT("ERR_FAILURE (%s : %s)", dir, strerror(errno)));
    retval = ERR_FAILURE;
    goto _retval;
  }

  while ((entry = readdir(dd)))
  {
    const char *name = entry->d_name;
    size_t      size = dir_size + strlen(name) + 2;

    if (!strcmp(name, ".") || !strcmp(name, "..")) { continue; }

    char full_path[size];
    snprintf(full_path, size, "%s/%s", dir, name);

    if (strstr(name, ".yar"))
    {
      if (IS_ERR_FAILURE(scanner_set_rule(scanner, full_path, name)))
      {
        LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE"));
        retval = ERR_FAILURE;
        goto _retval;
      }
    }

    if (entry->d_type == DT_DIR) { scanner_load_rules(scanner, full_path); }
  }

_retval:
  closedir(dd);
  return retval;
}

inline ERR
init_scanner(SCANNER **scanner, SCANNER_CONFIG config)
{
  ERR retval         = ERR_SUCCESS;
  *scanner           = malloc(sizeof(struct SCANNER));
  (*scanner)->config = config;

  ALLOC_ERR_FAILURE(*scanner);

  if (yr_initialize() != ERROR_SUCCESS)
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERROR_SUCCESS"));
    retval = ERR_FAILURE;
    goto _retval;
  }

  if (yr_compiler_create(&(*scanner)->yr_compiler) != ERROR_SUCCESS)
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERROR_SUCCESS"));
    retval = ERR_FAILURE;
    goto _retval;
  }

  if (IS_ERR_FAILURE(
              scanner_load_rules(*scanner, (*scanner)->config.yara.rules)))
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE"));
    retval = ERR_FAILURE;
    goto _retval;
  }

  if (yr_compiler_get_rules((*scanner)->yr_compiler, &(*scanner)->yr_rules) !=
      ERROR_SUCCESS)
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERROR_SUCCESS"));
    retval = ERR_FAILURE;
    goto _retval;
  }

_retval:
  return retval;
}

inline ERR
exit_scanner(SCANNER **scanner)
{
  ERR retval = ERR_SUCCESS;

  if (!scanner)
  {
    retval = ERR_FAILURE;
    goto _retval;
  }

  if (yr_finalize() != ERROR_SUCCESS)
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERROR_SUCCESS"));
    retval = ERR_FAILURE;
    goto _retval;
  }

  yr_compiler_destroy((*scanner)->yr_compiler);
  if ((*scanner)->yr_rules) yr_rules_destroy((*scanner)->yr_rules);

  if ((*scanner)->config.skip_dirs)
    del_skip_dirs(&(*scanner)->config.skip_dirs);

  free(*scanner);
  NO_USE_AFTER_FREE(*scanner);

_retval:
  return retval;
}
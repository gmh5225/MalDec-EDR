#define _GNU_SOURCE /* DT_DIR, DT_REG */

#include "scan/scan.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "err/err.h"
#include "logger/logger.h"
#include "scan/skip_dirs.h"

#define ROOT "/"

inline ERR
scan_file(SCANNER *scanner, YR_CALLBACK_FUNC callback)
{
  ERR                    retval    = ERR_SUCCESS;
  SCANNER_CALLBACK_ARGS *user_data = (struct SCANNER_CALLBACK_ARGS *)malloc(
          sizeof(struct SCANNER_CALLBACK_ARGS));

  ALLOC_ERR_FAILURE(user_data);

  user_data->file_path     = scanner->config.file_path;
  user_data->current_count = 0;
  user_data->verbose       = scanner->config.verbose;

  int code = yr_rules_scan_file(scanner->yr_rules, scanner->config.file_path,
                                SCAN_FLAGS_REPORT_RULES_MATCHING, callback,
                                user_data, 0);

  if (code != ERROR_SUCCESS || code == ERROR_INSUFFICIENT_MEMORY ||
      code == ERROR_COULD_NOT_MAP_FILE || code == ERROR_TOO_MANY_SCAN_THREADS ||
      code == ERROR_SCAN_TIMEOUT || code == ERROR_CALLBACK_ERROR ||
      code == ERROR_TOO_MANY_MATCHES)
  {
    retval = ERR_FAILURE;
  }

  free(user_data);
  NO_USE_AFTER_FREE(user_data);

  return retval;
}

inline ERR
scan_dir(SCANNER *scanner, YR_CALLBACK_FUNC callback, int32_t current_depth)
{
  int               retval = ERR_SUCCESS;
  SCANNER_CONFIG    config = scanner->config;
  struct SKIP_DIRS *skip   = config.skip;
  struct dirent    *entry;
  const char       *dir      = config.file_path;
  DIR              *dd       = opendir(dir);
  const size_t      dir_size = strlen(dir);
  const char       *fmt      = (!strcmp(dir, ROOT)) ? "%s%s" : "%s/%s";

  if (config.max_depth >= 0 && current_depth > config.max_depth) { goto ret; }
  else if (IS_NULL_PTR((dd)))
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %s : %d (%s)", dir, errno,
                                 strerror(errno)));
    retval = ERR_FAILURE;
    goto ret;
  }

  while ((entry = readdir(dd)) != NULL)
  {
    const char *name = entry->d_name;
    size_t      size = dir_size + strlen(name) + 2;

    if (!strcmp(name, ".") || !strcmp(name, "..") || get_skipped(&skip, dir))
    {
      continue;
    }

    char full_path[size];
    snprintf(full_path, size, fmt, dir, name);
    scanner->config.file_path = full_path;

    if (entry->d_type == DT_REG)
    {
      if (IS_ERR_FAILURE(scan_file(scanner, DEFAULT_SCAN_CALLBACK)))
      {
        retval = ERR_FAILURE;
      }
    }
    else if (entry->d_type == DT_DIR)
    {
      if (IS_ERR_FAILURE(
                  scan_dir(scanner, DEFAULT_SCAN_CALLBACK, current_depth + 1)))
      {
        retval = ERR_FAILURE;
      }
    }
  }

  closedir(dd);

ret:
  return retval;
}

inline ERR
scan(SCANNER *scanner)
{
  int            retval = ERR_SUCCESS;
  SCANNER_CONFIG config = scanner->config;

  struct stat st;
  int         fd = open(config.file_path, O_RDONLY);

  if (fstat(fd, &st) < 0)
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE  %s : (%s)", config.file_path,
                                 strerror(errno)));
    retval = ERR_FAILURE;
    goto ret;
  }

  mode_t mode = st.st_mode & S_IFMT;

  if (mode == S_IFDIR)
  {
    if (IS_ERR_FAILURE(scan_dir(scanner, DEFAULT_SCAN_CALLBACK, 0)))
    {
      retval = ERR_FAILURE;
      LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE"));
    }
  }
  else if (mode == S_IFREG)
  {
    if (IS_ERR_FAILURE(scan_file(scanner, DEFAULT_SCAN_CALLBACK)))
    {
      retval = ERR_FAILURE;
      LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE"));
    }
  }

ret:
  if (fd) close(fd);
  return retval;
}

void
scan_listen(SCANNER *scanner, INOTIFY *inotify)
{
  if (!IS_NULL_PTR(scanner) && !IS_NULL_PTR(inotify))
  {
    listen_to_events_inotify(&inotify, scanner, DEFAULT_SCAN_INOTIFY);
  }
}
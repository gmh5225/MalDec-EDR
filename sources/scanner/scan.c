#include "scanner/scanner.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "err/err.h"
#include "inotify/inotify.h"
#include "logger/logger.h"
#include "scanner/skip_dirs.h"

inline ERR
scan_file(SCANNER *scanner, YR_CALLBACK_FUNC callback)
{
#if DEBUG
  ALLOC_ERR_FAILURE(scanner->config.inspector);
  ALLOC_ERR_FAILURE(scanner->yr_compiler);
  ALLOC_ERR_FAILURE(scanner->yr_rules);
#endif

  ERR                    retval    = ERR_SUCCESS;
  SCANNER_CALLBACK_ARGS *user_data = (struct SCANNER_CALLBACK_ARGS *)malloc(
          sizeof(struct SCANNER_CALLBACK_ARGS));

  ALLOC_ERR_FAILURE(user_data);

  user_data->config        = scanner->config;
  user_data->current_count = 0;

  LOG_INFO(LOG_MESSAGE_FORMAT("Scanning '%s' ...", user_data->config.filepath));

  int code = yr_rules_scan_file(scanner->yr_rules, scanner->config.filepath,
                                (scanner->config.scan_type == QUICK_SCAN)
                                        ? SCAN_FLAGS_FAST_MODE
                                        : (SCAN_FLAGS_REPORT_RULES_MATCHING),
                                callback, user_data, 0);

  if (code == ERROR_INSUFFICIENT_MEMORY || code == ERROR_COULD_NOT_MAP_FILE ||
      code == ERROR_TOO_MANY_SCAN_THREADS || code == ERROR_SCAN_TIMEOUT ||
      code == ERROR_CALLBACK_ERROR || code == ERROR_TOO_MANY_MATCHES)
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
  int            retval = ERR_SUCCESS;
  SCANNER_CONFIG config = scanner->config;
  struct dirent *entry;
  DIR           *dd       = opendir(config.filepath);
  const size_t   dir_size = strlen(config.filepath);

  if (config.max_depth >= 0 && current_depth > config.max_depth)
  {
    goto _retval;
  }
  else if (IS_NULL_PTR((dd)))
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %s : %d (%s)", config.filepath,
                                 errno, strerror(errno)));
    retval = ERR_FAILURE;
    goto _retval;
  }

  while ((entry = readdir(dd)) != NULL)
  {
    const char *name = entry->d_name;
    size_t      size = dir_size + strlen(name) + 2;

    if (!strcmp(name, ".") || !strcmp(name, "..") ||
        get_skipped(&config.skip_dirs, config.filepath))
    {
      continue;
    }

    char fullpath[size];
    snprintf(fullpath, size, (!strcmp(config.filepath, "/")) ? "%s%s" : "%s/%s",
             config.filepath, name);
             
    scanner->config.filepath = fullpath;
    scanner->config.filename = name;

    if (entry->d_type == DT_REG) { retval = scan_file(scanner, callback); }
    else if (entry->d_type == DT_DIR)
    {
      retval = scan_dir(scanner, DEFAULT_SCAN_FILE, current_depth + 1);
    }

    (scanner->config.scan_type == QUICK_SCAN)
            ? usleep(2 * 10000)  // 20 milissegundos
            : usleep(6 * 10000); // 60 milissegundos
  }

  closedir(dd);

_retval:
  return retval;
}

ERR
scan_files_and_dirs(SCANNER *scanner)
{
#if DEBUG
  ALLOC_ERR_FAILURE(scanner->config.inspector);
  ALLOC_ERR_FAILURE(scanner->yr_compiler);
  ALLOC_ERR_FAILURE(scanner->yr_rules);
#endif

  ERR retval = ERR_SUCCESS;

  if (IS_NULL_PTR(scanner))
  {
    retval = ERR_FAILURE;
    goto _retval;
  }

  SCANNER_CONFIG config = scanner->config;

  int fd = open(config.filepath, O_RDONLY);
  if (fd == -1)
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE (open) %s (%s)", config.filepath,
                                 strerror(errno)));
    retval = ERR_FAILURE;
    goto _retval;
  }

  struct stat st;
  if (fstat(fd, &st) < 0)
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE (fstat) %s (%s)", config.filepath,
                                 strerror(errno)));
    retval = ERR_FAILURE;
    goto _close_fd;
  }

  const mode_t mode = st.st_mode & S_IFMT;

  if (mode == S_IFDIR)
  {
    // Remove '/' if passed argument contains '/'
    size_t size_filepath = strlen(config.filepath);
    if (size_filepath > 0 && config.filepath[size_filepath - 1] == '/')
      config.filepath[size_filepath - 1] = '\0';

    if (IS_ERR_FAILURE(scan_dir(scanner, DEFAULT_SCAN_FILE, 0)))
    {
      LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE (scan_dir)"));
      retval = ERR_FAILURE;
    }
  }
  else if (mode == S_IFREG)
  {
    // Adjust scanner config filename
    char *filename           = strrchr(config.filepath, '/');
    scanner->config.filename = (filename == NULL) ? config.filepath
                                                  : (filename + 1);
    if (IS_ERR_FAILURE(scan_file(scanner, DEFAULT_SCAN_FILE)))
    {
      LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE (scan_file)"));
      retval = ERR_FAILURE;
    }
  }

_close_fd:
  if (close(fd) == -1)
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE (close) %s (%s)", config.filepath,
                                 strerror(errno)));
    retval = ERR_FAILURE;
  }

_retval:
  return retval;
}

ERR
scan_listen_inotify(SCANNER *scanner)
{
#if DEBUG
  ALLOC_ERR_FAILURE(scanner->config.inotify);
  ALLOC_ERR_FAILURE(scanner->config.inspector);
  ALLOC_ERR_FAILURE(scanner->yr_compiler);
  ALLOC_ERR_FAILURE(scanner->yr_rules);
#endif

  ERR retval = ERR_FAILURE;
  if (!IS_NULL_PTR(scanner) && !IS_NULL_PTR(scanner->config.inotify))
  {
    retval = ERR_SUCCESS;
    listen_to_events_inotify(&scanner->config.inotify, scanner,
                             DEFAULT_SCAN_INOTIFY);
  }

  return retval;
}
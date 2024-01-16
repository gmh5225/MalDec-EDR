#define _GNU_SOURCE /* DT_DIR */

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <yara.h>

#include "err/err.h"
#include "logger/logger.h"
#include "scan.h"

inline int
default_scan_callback(YR_SCAN_CONTEXT *context, int message, void *message_data,
                      void *user_data)
{
  YR_RULE   *rule = (YR_RULE *)message_data;
  YR_STRING *string;
  YR_MATCH  *match;
  char      *strings_match      = NULL;
  size_t     strings_match_size = 0;

  switch (message)
  {
    case CALLBACK_MSG_SCAN_FINISHED:
      if (((SCANNER_CALLBACK_ARGS *)user_data)->verbose ||
          ((SCANNER_CALLBACK_ARGS *)user_data)->current_count)
        LOG_INFO("CALLBACK_MSG_SCAN_FINISHED : All rules were passed in this "
                 "file '%s', the scan is over, rules matching %d",
                 ((SCANNER_CALLBACK_ARGS *)user_data)->file_path,
                 ((SCANNER_CALLBACK_ARGS *)user_data)->current_count);
      break;

    case CALLBACK_MSG_RULE_MATCHING:
      ((SCANNER_CALLBACK_ARGS *)user_data)->current_count++;

      // allocate initial memory for strings_match
      strings_match_size = 1028;
      strings_match      = malloc(strings_match_size);

      // initialize strings_match to an empty string
      strings_match[0] = '\0';

      ALLOC_ERR_FAILURE(strings_match);

      yr_rule_strings_foreach(rule, string)
      {
        yr_string_matches_foreach(context, string, match)
        {
          size_t new_size =
                  strings_match_size + strlen(string->identifier) + 20;

          if (new_size > strings_match_size)
          {
            strings_match = realloc(strings_match, new_size);

            ALLOC_ERR_FAILURE(strings_match);

            strings_match_size = new_size;
          }
          snprintf(strings_match + strlen(strings_match),
                   new_size - strlen(strings_match), "[%s:0x%lx]",
                   string->identifier, match->offset);
        }
      }

      LOG_FATAL("CALLBACK_MSG_RULE_MATCHING : The rule '%s' were identified in "
                "this file '%s', Strings match %s",
                rule->identifier,
                ((SCANNER_CALLBACK_ARGS *)user_data)->file_path, strings_match);

      free(strings_match);
      NO_USE_AFTER_FREE(strings_match);

      break;

    case CALLBACK_MSG_RULE_NOT_MATCHING: break;
  }

  return CALLBACK_CONTINUE;
}

static inline void
decision_making_event_type(const struct inotify_event *event, SCANNER *scanner)
{
  if (event->mask & IN_ACCESS) { LOG_INFO("IN_ACCESS "); }
  else if (event->mask & IN_MODIFY)
  {
    LOG_INFO("IN_MODIFY ");
    goto scan;
  }
  else if (event->mask & IN_ATTRIB) { LOG_INFO("IN_ATTRIB "); }
  else if (event->mask & IN_CLOSE_WRITE)
  {
    LOG_INFO("IN_CLOSE_WRITE ");
    goto scan;
  }
  else if (event->mask & IN_CLOSE_NOWRITE) { LOG_INFO("IN_CLOSE_NOWRITE "); }
  else if (event->mask & IN_OPEN) { LOG_INFO("IN_OPEN "); }
  else if (event->mask & IN_MOVED_FROM) { LOG_INFO("IN_MOVED_FROM "); }
  else if (event->mask & IN_MOVED_TO) { LOG_INFO("IN_MOVED_TO "); }
  else if (event->mask & IN_MOVE) { LOG_INFO("IN_MOVE "); }
  else if (event->mask & IN_CREATE)
  {
    LOG_INFO("IN_CREATE ");
    goto scan;
  }
  else if (event->mask & IN_DELETE) { LOG_INFO("IN_DELETE "); }
  else if (event->mask & IN_DELETE_SELF) { LOG_INFO("IN_DELETE_SELF "); }
  else if (event->mask & IN_MOVE_SELF) { LOG_INFO("IN_MOVE_SELF "); }
  else if (event->mask & IN_UNMOUNT) { LOG_INFO("IN_UNMOUNT "); }
  else if (event->mask & IN_Q_OVERFLOW) { LOG_INFO("IN_Q_OVERFLOW "); }
  else if (event->mask & IN_IGNORED) { LOG_INFO("IN_IGNORED "); }
  else if (event->mask & IN_ONLYDIR) { LOG_INFO("IN_ONLYDIR "); }
  else if (event->mask & IN_DONT_FOLLOW) { LOG_INFO("IN_DONT_FOLLOW "); }
  else if (event->mask & IN_EXCL_UNLINK) { LOG_INFO("IN_EXCL_UNLINK "); }
  else if (event->mask & IN_MASK_CREATE) { LOG_INFO("IN_MASK_CREATE "); }
  else if (event->mask & IN_MASK_ADD) { LOG_INFO("IN_MASK_ADD "); }
  else if (event->mask & IN_ISDIR) { LOG_INFO("IN_ISDIR "); }
  else if (event->mask & IN_ONESHOT) { LOG_INFO("IN_ONESHOT "); }
  else
  {
    LOG_ERROR("ERR_MASK");
    // Handle the case when mask doesn't match any of the defined constants
  }

  return;

scan:
  if (IS_ERR_FAILURE(scan(scanner)))
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("Unable to scan the created file '%s' ",
                                 scanner->config.file_path));
  }
}

static inline void
log_watched_directory(INOTIFY *inotify, const struct inotify_event *event,
                      SCANNER *scanner)
{
  struct PATHS *paths = inotify->config.paths;
  for (size_t i = 0; i < inotify->config.quantity_fds;
       paths    = paths->hh.next, i++)
  {
    if (inotify->wd[i] == event->wd)
    {
      // TODO: add concat for scan file /<dir>/+<file>
      scanner->config.file_path = paths->path;
      if (event->len) { LOG_INFO("%s/%s", paths->path, event->name); }
      else
        LOG_INFO("%s", paths->path);

      break;
    }
  }
}

static inline void
process_inotify_events(INOTIFY *inotify, char *buf, ssize_t len,
                       SCANNER **scanner)
{
  const struct inotify_event *event = NULL;

  for (char *ptr = buf; ptr < buf + len;
       ptr += sizeof(struct inotify_event) + event->len)
  {
    event = (const struct inotify_event *)ptr;

    decision_making_event_type(event, *scanner);
    log_watched_directory(inotify, event, *scanner);

    if (event->mask & IN_ISDIR)
      LOG_INFO("[directory]\n");
    else
      LOG_INFO("[file]\n");
  }
}

inline void
default_scan_inotify(INOTIFY *inotify, void *buff)
{
  SCANNER *scanner = (SCANNER *)buff;

  char    buf[4096] __attribute__((aligned(__alignof__(struct inotify_event))));
  ssize_t len;

  for (;;)
  {
    len = read(inotify->fd_inotify, buf, sizeof(buf));
    if (len == -1 && errno != EAGAIN)
    {
      LOG_ERROR(LOG_MESSAGE_FORMAT("Failed to read from inotify %d (%s) ",
                                   errno, strerror(errno)));
    }
    if (len <= 0) break;

    process_inotify_events(inotify, buf, len, &scanner);
  }
}

inline static ERR
scanner_set_rule(SCANNER *scanner, const char *path, const char *yara_file_name)
{
  ERR                retval   = ERR_SUCCESS;
  YR_FILE_DESCRIPTOR rules_fd = open(path, O_RDONLY);

  if (yr_compiler_add_fd(scanner->yr_compiler, rules_fd, NULL,
                         yara_file_name) != ERROR_SUCCESS)
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
    goto ret;
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
        goto ret;
      }
    }

    if (entry->d_type == DT_DIR) { scanner_load_rules(scanner, full_path); }
  }

ret:
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
    goto ret;
  }

  if (yr_compiler_create(&(*scanner)->yr_compiler) != ERROR_SUCCESS)
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERROR_SUCCESS"));
    retval = ERR_FAILURE;
    goto ret;
  }

  if (IS_ERR_FAILURE(scanner_load_rules(*scanner, (*scanner)->config.rules)))
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE"));
    retval = ERR_FAILURE;
    goto ret;
  }

  if (yr_compiler_get_rules((*scanner)->yr_compiler, &(*scanner)->yr_rules) !=
      ERROR_SUCCESS)
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERROR_SUCCESS"));
    retval = ERR_FAILURE;
    goto ret;
  }

ret:
  return retval;
}

inline ERR
exit_scanner(SCANNER **scanner)
{
  ERR retval = ERR_SUCCESS;

  if (!scanner)
  {
    retval = ERR_FAILURE;
    goto ret;
  }

  if (yr_finalize() != ERROR_SUCCESS)
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERROR_SUCCESS"));
    retval = ERR_FAILURE;
    goto ret;
  }

  yr_compiler_destroy((*scanner)->yr_compiler);
  yr_rules_destroy((*scanner)->yr_rules);

  if ((*scanner)->config.skip) del_skip_dirs(&(*scanner)->config.skip);

  free(*scanner);
  NO_USE_AFTER_FREE(*scanner);

ret:
  return retval;
}
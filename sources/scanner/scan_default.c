#include "err/err.h"
#include "logger/logger.h"
#include "scanner.h"

#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>

static inline void
decision_making_event_type(const struct inotify_event **event,
                           SCANNER                    **scanner)
{
  if ((*event)->mask & IN_ACCESS) { LOG_INFO("IN_ACCESS "); }
  else if ((*event)->mask & IN_MODIFY)
  {
    LOG_WARN("IN_MODIFY ");
    goto scan;
  }
  else if ((*event)->mask & IN_CLOSE_WRITE)
  {
    LOG_WARN("IN_CLOSE_WRITE ");
    goto scan;
  }
  else if ((*event)->mask & IN_CREATE)
  {
    LOG_WARN("IN_CREATE ");
    goto scan;
  }

  return;

scan:
  if ((*scanner)->config.filepath)
  {
    if (IS_ERR_FAILURE(scan(*scanner)))
    {
      LOG_ERROR(LOG_MESSAGE_FORMAT("Unable to scan the created file '%s' ",
                                   (*scanner)->config.filepath));
    }
  }

  free((*scanner)->config.filepath);
  NO_USE_AFTER_FREE((*scanner)->config.filepath);
}

static inline void
watched_directory_event(INOTIFY **inotify, const struct inotify_event *event,
                        SCANNER *scanner)
{
  struct PATHS *paths = (*inotify)->config.paths;
  for (size_t i = 0; i < (*inotify)->config.quantity_fds;
       paths    = paths->hh.next, i++)
  {
    if ((*inotify)->wd[i] == event->wd)
    {
      if (event->len)
      {
        LOG_INFO("%s/%s", paths->path, event->name);

        size_t path_size =
                snprintf(NULL, 0, "%s/%s", paths->path, event->name) + 1;
        scanner->config.filepath = malloc(path_size);

        ALLOC_ERR_FAILURE(scanner->config.filepath);

        snprintf(scanner->config.filepath, path_size, "%s/%s", paths->path,
                 event->name);
      }
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

    watched_directory_event(&inotify, event, *scanner);
    decision_making_event_type(&event, &*scanner);

    if (event->mask & IN_ISDIR)
      LOG_INFO("[directory]");
    else
      LOG_INFO("[file]");
  }
}

/**
 * Scanner Defaults for Inotify and File Scanning
 * 
 * This code defines default configurations and behavior for a scanner
 * utilizing inotify for file system monitoring and scanning files for
 * potential threats or changes.
 */

inline void
default_scan_inotify(INOTIFY *inotify, void *buff)
{
  SCANNER                                         *scanner = (SCANNER *)buff;
  _Alignas(__alignof__(struct inotify_event)) char buf[4096];

  for (;;)
  {
    ssize_t len;
    len = read(inotify->fd_inotify, buf, sizeof(buf));

    if (len == -1 && errno != EAGAIN)
    {
      LOG_ERROR(LOG_MESSAGE_FORMAT("Failed to read from inotify %d (%s) ",
                                   errno, strerror(errno)));
    }

    if (len <= 0) { break; }

    process_inotify_events(inotify, buf, len, &scanner);
  }
}

inline int
default_scan_file(YR_SCAN_CONTEXT *context, int message, void *message_data,
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
      if (((SCANNER_CALLBACK_ARGS *)user_data)->config.verbose ||
          ((SCANNER_CALLBACK_ARGS *)user_data)->current_count)
        LOG_INFO("All rules were passed in this "
                 "file '%s', the scan is over, rules matching %d",
                 ((SCANNER_CALLBACK_ARGS *)user_data)->config.filepath,
                 ((SCANNER_CALLBACK_ARGS *)user_data)->current_count);
      break;

    case CALLBACK_MSG_RULE_MATCHING:
      ((SCANNER_CALLBACK_ARGS *)user_data)->current_count++;

      // allocate initial memory for strings_match
      strings_match_size = 1028;
      strings_match      = malloc(strings_match_size);
      ALLOC_ERR_FAILURE(strings_match);

      // initialize strings_match to an empty string
      strings_match[0] = '\0';

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

      LOG_FATAL("The rule '%s' were identified in "
                "this file '%s', Strings match %s",
                rule->identifier,
                ((SCANNER_CALLBACK_ARGS *)user_data)->config.filepath,
                strings_match);

      // add quarantine using inspector
      char path[PATH_MAX];

      if (IS_NULL_PTR(realpath(
                  ((SCANNER_CALLBACK_ARGS *)user_data)->config.filepath, path)))
      {
        LOG_ERROR(LOG_MESSAGE_FORMAT(
                "ERR_FAILURE Failed to resolve path '%s'.\n",
                ((SCANNER_CALLBACK_ARGS *)user_data)->config.filepath));
      }

      time_t           datatime = time(NULL);
      QUARANTINE_FILES file     = (QUARANTINE_FILES){
                  .filepath = path,
                  .detected = rule->identifier,
                  .filename = ((SCANNER_CALLBACK_ARGS *)user_data)->config.filename,
                  .datatime = ctime(&datatime)};

      if (IS_ERR_FAILURE(add_quarantine_inspector(
                  ((SCANNER_CALLBACK_ARGS *)user_data)->config.inspector,
                  &file)))
      {
        LOG_ERROR(LOG_MESSAGE_FORMAT(
                "ERR_FAILURE Failed to add file '%s' to quarantine.\n",
                ((SCANNER_CALLBACK_ARGS *)user_data)->config.filename));
      }

      free(strings_match);
      NO_USE_AFTER_FREE(strings_match);

      break;

    case CALLBACK_MSG_RULE_NOT_MATCHING: break;
  }

  return CALLBACK_CONTINUE;
}
#include "err/err.h"
#include "logger/logger.h"
#include "scanner.h"

#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>

inline void
default_scan_inotify(INOTIFY *inotify, void *buff)
{
  SCANNER                                         *scanner = (SCANNER *)buff;
  _Alignas(__alignof__(struct inotify_event)) char buf[4096];
  const struct inotify_event                      *event = NULL;

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

    for (char *ptr = buf; ptr < buf + len;
         ptr += sizeof(struct inotify_event) + event->len)
    {
      event = (const struct inotify_event *)ptr;
      (event->mask & IN_ISDIR) ? LOG_INFO("in_directory") : LOG_INFO("in_file");
      struct PATHS *paths = inotify->config.paths;

      for (size_t i = 0; i < inotify->config.quantity_fds;
           paths    = paths->hh.next, i++)
      {
        if (inotify->wd[i] == event->wd)
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
          else { LOG_INFO("%s", paths->path); }
          break;
        }
      }
      if (event->mask & IN_ACCESS) { LOG_INFO("in_access "); }
      else if (event->mask & IN_MODIFY || event->mask & IN_CLOSE_WRITE ||
               event->mask & IN_CREATE)
      {
        if (event->mask & IN_MODIFY) { LOG_WARN("in_modify "); }
        else if (event->mask & IN_CLOSE_WRITE) { LOG_WARN("in_close_write "); }
        else if (event->mask & IN_CREATE) { LOG_WARN("in_create "); }
        sleep(1);
        if (scanner->config.filepath)
        {
          SCANNER cscanner =
                  *scanner; // Copy scanner for function scan_files_and_dirs
          if (IS_ERR_FAILURE(scan_files_and_dirs(&cscanner)))
          {
            LOG_ERROR(LOG_MESSAGE_FORMAT("Unable to scan the created file "
                                         "'%s' ",
                                         scanner->config.filepath));
          }
        }
        free(scanner->config.filepath);
        NO_USE_AFTER_FREE(scanner->config.filepath);
      }
    }
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
      {
        LOG_INFO(LOG_MESSAGE_FORMAT(
                "All rules were passed in this "
                "file '%s', the scan is over, rules matching %d file is "
                "suspect",
                ((SCANNER_CALLBACK_ARGS *)user_data)->config.filepath,
                ((SCANNER_CALLBACK_ARGS *)user_data)->current_count));
      }
      break;

    case CALLBACK_MSG_RULE_MATCHING:
      ((SCANNER_CALLBACK_ARGS *)user_data)->current_count++;

      if (((SCANNER_CALLBACK_ARGS *)user_data)->config.verbose)
      {
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

        LOG_FATAL(LOG_MESSAGE_FORMAT(
                "The rule '%s' were identified in "
                "this file '%s', Strings match %s",
                rule->identifier,
                ((SCANNER_CALLBACK_ARGS *)user_data)->config.filepath,
                strings_match));
      }
      else
      {
        LOG_FATAL(LOG_MESSAGE_FORMAT(
                "File '%s' is malicious",
                ((SCANNER_CALLBACK_ARGS *)user_data)->config.filepath));
      }
      // add quarantine using inspector
      char path[PATH_MAX];

      if (IS_NULL_PTR(realpath(
                  ((SCANNER_CALLBACK_ARGS *)user_data)->config.filepath, path)))
      {
        LOG_ERROR(LOG_MESSAGE_FORMAT(
                "Failed to resolve path '%s'",
                ((SCANNER_CALLBACK_ARGS *)user_data)->config.filepath));
      }

      time_t           datetime = time(NULL);
      QUARANTINE_FILE file     = (QUARANTINE_FILE){
                  .filepath = path,
                  .detected = rule->identifier,
                  .filename = ((SCANNER_CALLBACK_ARGS *)user_data)->config.filename,
                  .datetime = ctime(&datetime)};

      LOG_INFO(LOG_MESSAGE_FORMAT("Inserting quarantine file '%s' suspect",
                                  file.filepath));
      if (IS_ERR_FAILURE(add_quarantine_inspector(
                  ((SCANNER_CALLBACK_ARGS *)user_data)->config.inspector,
                  &file)))
      {
        LOG_ERROR(LOG_MESSAGE_FORMAT(
                "ERR_FAILURE Failed to add file '%s' to quarantine.",
                ((SCANNER_CALLBACK_ARGS *)user_data)->config.filename));
      }

      if (((SCANNER_CALLBACK_ARGS *)user_data)->config.verbose)
      {
        free(strings_match);
        NO_USE_AFTER_FREE(strings_match);
      }

      break;

    case CALLBACK_MSG_RULE_NOT_MATCHING: break;
  }

  return CALLBACK_CONTINUE;
}
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
                 "file "
                 "'%s', the scan is over, rules matching %d",
                 ((SCANNER_CALLBACK_ARGS *)user_data)->file_path,
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

      LOG_FATAL("CALLBACK_MSG_RULE_MATCHING : The rule '%s' were identified in "
                "this "
                "file '%s', Strings match %s",
                rule->identifier,
                ((SCANNER_CALLBACK_ARGS *)user_data)->file_path, strings_match);

      free(strings_match);
      NO_USE_AFTER_FREE(strings_match);

      break;

    case CALLBACK_MSG_RULE_NOT_MATCHING: break;
  }

  return CALLBACK_CONTINUE;
}

inline void
default_scan_inotify(INOTIFY *inotify, void *buff)
{
  SCANNER *scanner = (SCANNER *)buff;

  char buf[4096] __attribute__((aligned(__alignof__(struct inotify_event))));
  const struct inotify_event *event;
  ssize_t                     len;

  for (;;)
  {
    len = read((*inotify).fd_inotify, buf, sizeof(buf));
    if (len == -1 && errno != EAGAIN)
    {
      perror("read");
      exit(EXIT_FAILURE);
    }

    /* If the nonblocking read() found no events to read, then
                  it returns -1 with errno set to EAGAIN. In that case,
                  we exit the loop. */

    if (len <= 0) break;

    /* Loop over all events in the buffer. */

    for (char *ptr = buf; ptr < buf + len;
         ptr += sizeof(struct inotify_event) + event->len)
    {
      event = (const struct inotify_event *)ptr;

      /* Print event type. */

      if (event->mask & IN_OPEN) printf("IN_OPEN: ");
      if (event->mask & IN_CLOSE_NOWRITE) printf("IN_CLOSE_NOWRITE: ");
      if (event->mask & IN_CLOSE_WRITE) printf("IN_CLOSE_WRITE: ");

      /* Print the name of the watched directory. */

      for (size_t i = 0; i < (*inotify).config.quantity_fds; ++i)
      {
        if ((*inotify).wd[i] == event->wd)
        {
          printf("%s/", (*inotify).config.paths[i]);
          break;
        }
      }
      if (event->len) printf("%s", event->name);

      if (event->mask & IN_ISDIR)
        printf(" [directory]\n");
      else
        printf(" [file]\n");
    }
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
  return 0;
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
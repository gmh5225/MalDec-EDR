#define _GNU_SOURCE /* DT_DIR */

#include <scan/scan.h>

#include <yara.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include "err/err.h"
#include "logger/logger.h"

#define RULES_FOLDER "../../../rules/YARA-Mindshield-Analysis"

int default_scan_callback(YR_SCAN_CONTEXT *context,
                          int message,
                          void *message_data,
                          void *user_data)
{
    YR_RULE *rule = (YR_RULE *)message_data;
    YR_STRING *string;
    YR_MATCH *match;
    char *strings_match = NULL;
    size_t strings_match_size = 0;

    switch (message)
    {
    case CALLBACK_MSG_SCAN_FINISHED:
        if (((CALLBACK_ARGS *)user_data)->verbose || ((CALLBACK_ARGS *)user_data)->current_count)
            LOG_INFO("Yara : All rules were passed in this file '%s', the scan is over, rules matching %d",
                     ((CALLBACK_ARGS *)user_data)->file_path, ((CALLBACK_ARGS *)user_data)->current_count);
        break;

    case CALLBACK_MSG_RULE_MATCHING:
        ((CALLBACK_ARGS *)user_data)->current_count++;

        // allocate initial memory for strings_match
        strings_match_size = 1028;
        strings_match = malloc(strings_match_size);
        if (strings_match == NULL)
        {
            LOG_ERROR("Yara : Memory allocation error\n");
            exit(EXIT_FAILURE);
        }

        // initialize strings_match to an empty string
        strings_match[0] = '\0';

        yr_rule_strings_foreach(rule, string)
        {
            yr_string_matches_foreach(context, string, match)
            {
                size_t new_size = strings_match_size + strlen(string->identifier) + 20;

                if (new_size > strings_match_size)
                {
                    strings_match = realloc(strings_match, new_size);
                    if (strings_match == NULL)
                    {
                        LOG_ERROR("Yara : Memory reallocation error\n");
                        exit(EXIT_FAILURE);
                    }
                    strings_match_size = new_size;
                }
                snprintf(strings_match + strlen(strings_match), new_size - strlen(strings_match), "[%s:0x%lx]", string->identifier, match->offset);
            }
        }

        LOG_WARN("Yara : The rule '%s' were identified in this file '%s', Strings match %s",
                 rule->identifier, ((CALLBACK_ARGS *)user_data)->file_path, strings_match);

        free(strings_match);
        break;

    case CALLBACK_MSG_RULE_NOT_MATCHING:
        break;
    }

    return CALLBACK_CONTINUE;
}

static int scanner_set_rule(SCANNER *scanner, const char *path, const char *yara_file_name)
{
    int retval = SUCCESS;
    YR_FILE_DESCRIPTOR rules_fd = open(path, O_RDONLY);

    if (yr_compiler_add_fd(scanner->yr_compiler, rules_fd, NULL, yara_file_name))
    {
        LOG_ERROR("Yara : yr_compiler_add_fd ERROR");
        retval = ERROR;
        goto ret;
    }

ret:
    close(rules_fd);
    return retval;
}

static int scanner_load_rules(SCANNER *scanner, const char *dir)
{
    int retval = SUCCESS;
    DIR *dd;
    struct dirent *entry;
    const size_t dir_size = strlen(dir);

    if ((dd = opendir(dir)) == NULL)
    {
        LOG_ERROR("Yara : scanner_load_rules ERROR (%s : %s)", dir, strerror(errno));
        retval = ERROR;
        goto ret;
    }

    while ((entry = readdir(dd)))
    {
        const char *name = entry->d_name;
        size_t size = dir_size + strlen(name) + 2;

        if (!strcmp(name, ".") || !strcmp(name, ".."))
        {
            continue;
        }

        char full_path[size];
        snprintf(full_path, size, "%s/%s", dir, name);

        if (strstr(name, ".yar"))
        {
            if (scanner_set_rule(scanner, full_path, name))
            {
                LOG_ERROR("Yara : scanner_set_rule() ERROR");
                retval = ERROR;
                goto ret;
            }
        }

        if (entry->d_type == DT_DIR)
        {
            scanner_load_rules(scanner, full_path);
        }
    }

ret:
    closedir(dd);
    return 0;
}

// ! TODO !
// - Add logs
// - Read config from file
int init_scanner(SCANNER **scanner, SCANNER_CONFIG config)
{
    int retval = SUCCESS;
    *scanner = malloc(sizeof(SCANNER));

    if (yr_initialize())
    {
        LOG_ERROR("Yara : yr_initialize() ERROR");
        retval = ERROR;
        goto ret;
    }

    if (yr_compiler_create(&(*scanner)->yr_compiler))
    {
        LOG_ERROR("Yara : yr_compiler_create() ERROR");
        retval = ERROR;
        goto ret;
    }

    if (scanner_load_rules(*scanner, RULES_FOLDER))
    {
        LOG_ERROR("Yara : scanner_set_rule() ERROR");
        retval = ERROR;
        goto ret;
    }

    if (yr_compiler_get_rules((*scanner)->yr_compiler, &(*scanner)->yr_rules))
    {
        LOG_ERROR("Yara : yr_compiler_create() ERROR");
        retval = ERROR;
        goto ret;
    }

ret:
    (*scanner)->config = config;
    return retval;
}

int exit_scanner(SCANNER **scanner)
{
    int retval = SUCCESS;
    if (!scanner)
    {
        retval = ERROR;
        goto ret;
    }

    if (yr_finalize())
    {
        LOG_ERROR("Yara : yr_finalize() ERROR");
        retval = ERROR;
        goto ret;
    }

    yr_compiler_destroy((*scanner)->yr_compiler);
    yr_rules_destroy((*scanner)->yr_rules);

    if ((*scanner)->config.skip)
        del_skip_dirs(&(*scanner)->config.skip);

    free(*scanner);

ret:
    return retval;
}
#define _GNU_SOURCE /* DT_DIR */

#include <scan/scan.h>

#include <yara.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include "err/err.h"

#define RULES_FOLDER "../../../rules/YARA-Mindshield-Analysis"

int default_scan_callback(YR_SCAN_CONTEXT *context,
                          int message,
                          void *message_data,
                          void *user_data)
{
    YR_RULE *rule = (YR_RULE *)(message_data);
    YR_STRING *string;
    YR_MATCH *match;

    switch (message)
    {
    case CALLBACK_MSG_RULE_NOT_MATCHING:
        break;
    case CALLBACK_MSG_RULE_MATCHING:
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
        fprintf(stderr, "Yara : yr_compiler_add_fd ERROR\n");
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
        fprintf(stderr, "Yara : scanner_load_rules ERROR (%s : %s)\n", dir, strerror(errno));
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
                fprintf(stderr, "Yara : scanner_set_rule() ERROR\n");
                retval = ERROR;
                goto ret;
            }
        }

        if (entry->d_type == DT_DIR)
        {
            // scanner_load_rules(scanner, full_path);
        }
    }

ret:
    closedir(dd);
    return 0;
}

// ! TODO !
// - Add logs
// - Read config from file
int scanner_init(SCANNER **scanner, SCANNER_CONFIG config)
{
    int retval = SUCCESS;
    *scanner = malloc(sizeof(SCANNER));

    if (yr_initialize())
    {
        fprintf(stderr, "Yara : yr_initialize() ERROR\n");
        retval = ERROR;
        goto ret;
    }

    if (yr_compiler_create(&(*scanner)->yr_compiler))
    {
        fprintf(stderr, "Yara : yr_compiler_create() ERROR\n");
        retval = ERROR;
        goto ret;
    }

    if (scanner_load_rules(*scanner, RULES_FOLDER))
    {
        fprintf(stderr, "Yara : scanner_set_rule() ERROR\n");
        retval = ERROR;
        goto ret;
    }

    if (yr_compiler_get_rules((*scanner)->yr_compiler, &(*scanner)->yr_rules))
    {
        fprintf(stderr, "Yara : yr_compiler_create() ERROR\n");
        retval = ERROR;
        goto ret;
    }

ret:
    (*scanner)->config = config;
    return retval;
}

int scanner_destroy(SCANNER **scanner)
{
    int retval = SUCCESS;
    if (!scanner)
    {
        retval = ERROR;
        goto ret;
    }

    YR_COMPILER *compiler = (*scanner)->yr_compiler;
    YR_RULES *rules = (*scanner)->yr_rules;

    if (yr_finalize())
    {
        fprintf(stderr, "Yara : yr_finalize() ERROR\n");
        retval = ERROR;
        goto ret;
    }

    if (compiler)
        yr_compiler_destroy(compiler);

    if (rules)
        yr_rules_destroy(rules);

    if ((*scanner)->config.skip)
        del_skip_dirs(&(*scanner)->config.skip);

    free(*scanner);

ret:
    return retval;
}
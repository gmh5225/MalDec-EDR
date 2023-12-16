#define _GNU_SOURCE /* DT_DIR, DT_REG */

#include "scan/scan.h"
#include "scan/skip_dirs.h"
#include "err/err.h"
#include "logger/logger.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define ROOT "/"

inline int scan_file(SCANNER *scanner, YR_CALLBACK_FUNC callback)
{
    CALLBACK_ARGS *user_data = (struct CALLBACK_ARGS *)malloc(sizeof(struct CALLBACK_ARGS));

    ALLOC_ERR(user_data);

    user_data->file_path = scanner->config.file_path;
    user_data->current_count = 0;
    user_data->verbose = false;

    yr_rules_scan_file(scanner->yr_rules, scanner->config.file_path, SCAN_FLAGS_REPORT_RULES_MATCHING, callback, user_data, 0);

    free(user_data);
}

int scan_dir(SCANNER *scanner, YR_CALLBACK_FUNC callback, int32_t currrent_depth)
{
    int retval = SUCCESS;
    DIR *dd;
    SCANNER_CONFIG config = scanner->config;
    struct dirent *entry;
    const char *dir = config.file_path;
    struct skip_dirs *skip = config.skip;
    const size_t dir_size = strlen(dir);
    const char *fmt = (!strcmp(dir, ROOT)) ? "%s%s" : "%s/%s";

    if (config.max_depth >= 0 && currrent_depth > config.max_depth)
    {
        retval = ERROR;
        goto ret;
    }
    else if (NULL_PTR((dd = opendir(dir))))
    {
        LOG_ERROR("Yara : scan_dir ERROR %s : %d (%s)", dir, errno, strerror(errno));
        retval = ERROR;
        goto ret;
    }

    while ((entry = readdir(dd)) != NULL)
    {
        const char *name = entry->d_name;
        size_t size = dir_size + strlen(name) + 2;

        if (!strcmp(name, ".") || !strcmp(name, "..") || get_skipped(skip, dir))
        {
            continue;
        }

        char full_path[size];
        snprintf(full_path, size, fmt, dir, name);
        scanner->config.file_path = full_path;

        if (entry->d_type == DT_REG)
        {
            int code = scan_file(scanner, DEFAULT_SCAN_CALLBACK);

            if (code < 0)
            {
                LOG_ERROR("Yara : scan_file ERROR %s : %d (%s)", full_path, code, strerror(errno));
            }
        }
        else if (entry->d_type == DT_DIR)
        {
            if (IS_ERR(scan_dir(scanner, DEFAULT_SCAN_CALLBACK, currrent_depth + 1)))
                ;
        }
    }

    closedir(dd);

ret:
    return retval;
}

int scan(SCANNER *scanner)
{
    int retval = SUCCESS;
    SCANNER_CONFIG config = scanner->config;

    struct stat st;
    int fd = open(config.file_path, O_RDONLY);

    if (fstat(fd, &st) < 0)
    {
        LOG_ERROR("Yara : scan ERROR %s : (%s)", config.file_path, strerror(errno));
        retval = ERROR;
        goto ret;
    }

    mode_t mode = st.st_mode & S_IFMT;

    if (mode == S_IFDIR)
    {
        if (IS_ERR(scan_dir(scanner, DEFAULT_SCAN_CALLBACK, 0)))
            ;
    }
    else if (mode == S_IFREG)
    {
        if (IS_ERR(scan_file(scanner, DEFAULT_SCAN_CALLBACK)))
            ;
    }

ret:
    close(fd);
    return retval;
}
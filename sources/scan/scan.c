#define _GNU_SOURCE /* DT_DIR, DT_REG */

#include "scan/scan.h"
#include "scan/skip_dirs.h"

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
    return yr_rules_scan_file(scanner->yr_rules, scanner->config.file_path, SCAN_FLAGS_REPORT_RULES_MATCHING, callback, NULL, 0);
}

int scan_dir(SCANNER *scanner, YR_CALLBACK_FUNC callback, int32_t __currrent_depth)
{

    DIR *dd;
    SCANNER_CONFIG config = scanner->config;
    struct dirent *entry;
    const char *dir = config.file_path;
    struct skip_dirs *skip = config.skip;

    const size_t dir_size = strlen(dir);
    const char *fmt = (!strcmp(dir, ROOT)) ? "%s%s" : "%s/%s";

    if (config.max_depth >= 0 && __currrent_depth > config.max_depth) return 0;

    if ((dd = opendir(dir)) == NULL)
    {
        fprintf(stderr, "Yara : scan_dir ERROR %s : %d (%s)\n", dir, errno, strerror(errno));
        return -1;
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

        // ADVANCED DEBUG TECHNIQUE
        // printf(">> %s\n", scanner->config.file_path);

        if (entry->d_type == DT_REG)
        {
            int code = scan_file(scanner, DEFAULT_SCAN_CALLBACK);

            if (code < 0)
            {
                fprintf(stderr, "Yara : scan_file ERROR %s : %d (%s)\n", full_path, code, strerror(errno));
                // return -1;
            }
        }
        else if (entry->d_type == DT_DIR)
        {
            scan_dir(scanner, DEFAULT_SCAN_CALLBACK, __currrent_depth + 1);
        }
    }

    closedir(dd);
    return 0;
}

int scan(SCANNER *scanner)
{
    SCANNER_CONFIG config = scanner->config;

    struct stat st;
    int fd = open(config.file_path, O_RDONLY);

    if (fstat(fd, &st) < 0)
    {
        fprintf(stderr, "scan : ERROR %s : (%s)\n", config.file_path, strerror(errno));
        return -1;
    }

    mode_t mode = st.st_mode & S_IFMT;
    close(fd);

    if (mode == S_IFDIR)
    {
        scan_dir(scanner, DEFAULT_SCAN_CALLBACK, 0);
    } else if (mode == S_IFREG) {
        scan_file(scanner, DEFAULT_SCAN_CALLBACK);
    }

    return 0;
}
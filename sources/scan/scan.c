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

int scan_file(SCANNER *scanner, const char *file, YR_CALLBACK_FUNC callback)
{
    int fd = open(file, O_RDONLY);

    int err = yr_rules_scan_fd(scanner->yr_rules, fd, SCAN_FLAGS_REPORT_RULES_MATCHING, callback, NULL, 0);

    close(fd);

    return err;
}

int scan_dir(SCANNER *scanner, const char *dir, YR_CALLBACK_FUNC callback, struct skip_dirs *skip)
{

    DIR *dd;
    struct dirent *entry;
    const size_t dir_size = strlen(dir);
    const char *fmt = (!strcmp(dir, ROOT)) ? "%s%s" : "%s/%s";

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

        // ADVANCED DEBUG TECHNIQUE
        // printf(">> %s\n", full_path);

        if (entry->d_type == DT_REG)
        {
            int code = 0;
            if ((code = scan_file(scanner, full_path, DEFAULT_SCAN_CALLBACK)))
            {
                fprintf(stderr, "Yara : scan_file ERROR %s : %d (%s)\n", full_path, code, strerror(errno));
                return -1;
            }
        } else if (entry->d_type == DT_DIR) {
            scan_dir(scanner, full_path, DEFAULT_SCAN_CALLBACK, skip);
        }
    }

    closedir(dd);
    return 0;
}
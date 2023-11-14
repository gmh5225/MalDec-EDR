#define _GNU_SOURCE /* DT_DIR, DT_REG */

#include <scan/scan.h>
#include <scan/ignored_dirs.h>


#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define ROOT "/"

struct ignored_dirs_list *IGNORE_DIR = NULL;

int scan_file(SCANNER *scanner, const char *file, YR_CALLBACK_FUNC callback)
{
    int fd = open(file, O_RDONLY);

    int err = yr_rules_scan_fd(scanner->yr_rules, fd, SCAN_FLAGS_REPORT_RULES_MATCHING, callback, NULL, 0);

    close(fd);

    return err;
}

int scan_folder(SCANNER *scanner, const char *folder, YR_CALLBACK_FUNC callback)
{

    DIR *dir = opendir(folder);
    if (!dir)
    {
        fprintf(stderr, "Yara : scan_folder ERROR %s : %d (%s)\n", folder, errno, strerror(errno));
        return -1;
    }

    const size_t folder_size = strlen(folder);

    add_ignore_dirs_from_list(
        &IGNORE_DIR,
        (const char *[]){ ".", "..", "sys", "proc", "run", "dev", "boot" }, 7);

    struct dirent *entry;
    const char *fmt = (strcmp(folder, ROOT) == 0) ? "%s%s" : "%s/%s";
    
    while ((entry = readdir(dir)) != NULL)
    {
        const char *name = entry->d_name;
        size_t size = folder_size+strlen(name)+2;
        
        if(find_ignored(&IGNORE_DIR, name))
        {
            continue;
        }

        char full_path[size];
        snprintf(full_path, size, fmt, folder, name);

        // ADVANCED DEBUG TECHNIQUE
        // printf("%s\n", full_path);

        if (entry->d_type == DT_REG)
        {
            struct stat s;

            int code = 0;
            if ((code = scan_file(scanner, full_path, DEFAULT_SCAN_CALLBACK)))
            {
                fprintf(stderr, "Yara : scan_file ERROR %s : %d (%s)\n", full_path, code, strerror(errno));
                return -1;
            }
        } else if (entry->d_type == DT_DIR) {
            scan_folder(scanner, full_path, DEFAULT_SCAN_CALLBACK);
        }
    }

    closedir(dir);
    return 0;
}
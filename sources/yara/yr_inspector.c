#define _GNU_SOURCE /* DT_DIR */

#include <yr_inspector.h>

#include <yara.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include<string.h>

#define RULES_FOLDER "../rules/YARA-Mindshield-Analysis"

typedef struct INSPECTOR {
    YR_RULES *yr_rules;
    YR_COMPILER *yr_compiler;
} INSPECTOR;

int default_scan_callback(YR_SCAN_CONTEXT *context,
                                        int message,
                                        void *message_data,
                                        void *user_data)
{
    YR_RULE *rule = (YR_RULE*)(message_data);
    YR_STRING *string;
    YR_MATCH *match;

    switch (message)
    {
    case CALLBACK_MSG_SCAN_FINISHED:
        printf("FINISHED: %d\n", message);
        break;
    case CALLBACK_MSG_RULE_MATCHING:
        printf("MATCHED: %d\n", message);
        break;
    }

    return CALLBACK_CONTINUE;
}

static int inspector_set_rule(INSPECTOR *inspector, const char *path, const char *yara_file_name)
{
    YR_FILE_DESCRIPTOR rules_fd = open(path, O_RDONLY);

    if (yr_compiler_add_fd(inspector->yr_compiler, rules_fd, NULL, yara_file_name))
    {
        fprintf(stderr, "Yara : yr_compiler_add_fd ERROR\n");
        return -1;
    }

    close(rules_fd);

    return 0;
}

static int inspector_set_rules_from_dir(INSPECTOR *inspector, const char *folder)
{
    DIR *dir = opendir(folder);
    if (!dir)
    {
        fprintf(stderr, "Yara : inspector_set_rules_from_dir ERROR (%s : %s)\n", folder, strerror(errno));
        return -1;
    }

    const size_t folder_size = strlen(folder);
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL)
    {
        const char *name = entry->d_name;
        size_t size = folder_size+strlen(name)+2;
        
        if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
        {
            continue;
        }

        char full_path[size];
        snprintf(full_path, size, "%s/%s", folder, name);

        if (strstr(name, ".yar"))
        {
            if (inspector_set_rule(inspector, full_path, name))
            {
                fprintf(stderr, "Yara : inspector_set_rule() ERROR\n");
                return -1;
            }
        }
        
        if (entry->d_type == DT_DIR)
        {
            inspector_set_rules_from_dir(inspector, full_path);
        }
    }

    closedir(dir);
    return 0;
}

// ! TODO !
// - Add logs
int inspector_init(INSPECTOR **inspector)
{
    *inspector = malloc(sizeof(INSPECTOR));

    if (yr_initialize())
    {
        fprintf(stderr, "Yara : yr_initialize() ERROR\n");
        return -1;
    }

    if (yr_compiler_create(&(*inspector)->yr_compiler))
    {
        fprintf(stderr, "Yara : yr_compiler_create() ERROR\n");
        return -1;
    }

    if (inspector_set_rules_from_dir(*inspector, RULES_FOLDER))
    {
        fprintf(stderr, "Yara : inspector_set_rule() ERROR\n");
        return -1;
    }

    if (yr_compiler_get_rules((*inspector)->yr_compiler, &(*inspector)->yr_rules))
    {
        fprintf(stderr, "Yara : yr_compiler_create() ERROR\n");
        return -1;
    }

    return 0;
} 

int inspector_scan_file(INSPECTOR *inspector, const char *file, YR_CALLBACK_FUNC callback)
{
    int fd = open(file, O_RDONLY);

    int err = yr_rules_scan_fd(inspector->yr_rules, fd, SCAN_FLAGS_REPORT_RULES_MATCHING, callback, NULL, 0);

    close(fd);

    return err;
}
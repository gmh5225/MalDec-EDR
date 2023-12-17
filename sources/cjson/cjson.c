#include "cjson.h"
#include "err/err.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
 #include <string.h>
#include <errno.h>

int init_json(struct json_object **json_obj, const char *filename)
{
    int fd = open(filename, O_RDONLY);
    int retval = SUCCESS;

    if (fd == -1)
    {
        fprintf(stderr, "Cjson : Error opening file ('%s') : %s \n", filename, strerror(errno));
        retval = ERROR;
        goto ret;
    }

    struct stat file_stat;
    if (fstat(fd, &file_stat) == -1)
    {
        fprintf(stderr, "Cjson : Error getting file stats ('%s') : %s\n", filename, strerror(errno));
        retval = ERROR;
        goto ret;
    }

    char *json_string = malloc(file_stat.st_size + 1);
    if (json_string == NULL)
    {
        fprintf(stderr, "Cjson : Error allocating memory for JSON string ('%s') : %s \n", filename, strerror(errno));
        retval = ERROR;
        goto ret;
    }

    ssize_t bytes_read = read(fd, json_string, file_stat.st_size);

    if (bytes_read == -1)
    {
        perror("Cjson : Error reading file");
        fprintf(stderr, "'%s'\n", filename);
        retval = ERROR;
        goto ret;
    }

    json_string[file_stat.st_size] = '\0';

    *json_obj = json_tokener_parse(json_string);
    if (IS_NULL_PTR(*json_obj))
    {
        fprintf(stderr, "Cjson : Error parsing JSON in file %s\n", filename);
        retval = ERROR;
        goto ret;
    }

ret:
    if (!IS_NULL_PTR(json_string))
    {
        free(json_string);
        NO_USE_AFTER_FREE(json_string);
    }
    close(fd);

    return retval;
}

void exit_json(struct json_object **json_obj)
{
    json_object_put(*json_obj);
}
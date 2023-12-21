#include "cjson.h"
#include "err/err.h"
#include "logger/logger.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

inline ERR init_json(struct json_object **json_obj, const char *filename)
{
    int fd = open(filename, O_RDONLY);
    int retval = ERR_SUCCESS;
    char *json_string = NULL;

    if (fd == -1)
    {
        fprintf(stderr, LOG_MESSAGE_FORMAT("Error opening file ('%s') : %s \n", filename, strerror(errno)));
        retval = ERR_FAILURE;
        goto ret;
    }

    struct stat file_stat;
    if (fstat(fd, &file_stat) == -1)
    {
        fprintf(stderr, LOG_MESSAGE_FORMAT("Error getting file stats ('%s') : %s\n", filename, strerror(errno)));
        retval = ERR_FAILURE;
        goto ret;
    }

    json_string = malloc(file_stat.st_size + 1);
    if (json_string == NULL)
    {
        fprintf(stderr, LOG_MESSAGE_FORMAT("Error allocating memory for JSON string ('%s') : %s \n", filename, strerror(errno)));
        retval = ERR_FAILURE;
        goto ret;
    }

    ssize_t bytes_read = read(fd, json_string, file_stat.st_size);

    if (bytes_read == -1)
    {
        fprintf(stderr, LOG_MESSAGE_FORMAT("Error reading file ('%s') : %s \n", filename, strerror(errno)));
        retval = ERR_FAILURE;
        goto ret;
    }

    json_string[file_stat.st_size] = '\0';

    *json_obj = json_tokener_parse(json_string);
    if (IS_NULL_PTR(*json_obj))
    {
        fprintf(stderr, LOG_MESSAGE_FORMAT("Error parsing JSON in file %s\n", filename));
        retval = ERR_FAILURE;
        goto ret;
    }

ret:
    if (!IS_NULL_PTR(json_string))
    {
        free(json_string);
        NO_USE_AFTER_FREE(json_string);
    }

    if (fd)
        close(fd);

    return retval;
}

inline void exit_json(struct json_object **json_obj)
{
    json_object_put(*json_obj);
}
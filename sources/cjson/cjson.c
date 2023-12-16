#include "cjson.h"
#include "err/err.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>


int init_json(struct json_object **json_obj, const char *filename)
{
    int fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        perror("Cjson : Error opening file");
        fprintf(stderr, "File: %s\n", filename);
        return ERROR;
    }

    struct stat file_stat;
    if (fstat(fd, &file_stat) == -1)
    {
        perror("Cjson : Error getting file stats");
        fprintf(stderr, "File: %s\n", filename);
        close(fd);
        return ERROR;
    }

    char *json_string = malloc(file_stat.st_size + 1);
    if (json_string == NULL)
    {
        perror("Cjson : Error allocating memory for JSON string");
        fprintf(stderr, "File: %s\n", filename);
        close(fd);
        return ERROR;
    }

    ssize_t bytes_read = read(fd, json_string, file_stat.st_size);
    close(fd);

    if (bytes_read == -1)
    {
        perror("Cjson : Error reading file");
        fprintf(stderr, "File: %s\n", filename);
        free(json_string);
        return ERROR;
    }

    json_string[file_stat.st_size] = '\0';

    *json_obj = json_tokener_parse(json_string);
    if (*json_obj == NULL)
    {
        fprintf(stderr, "Cjson : Error parsing JSON in file %s\n", filename);
        return ERROR;
    }

    free(json_string);
}

void exit_json(struct json_object **json_obj)
{
    json_object_put(*json_obj);
}
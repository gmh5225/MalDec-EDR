#pragma once

#include <stdlib.h>

#define IS_ERR(err) err == ERROR
#define IS_MALLOC_CHECK(ptr)                                    \
    do                                                          \
    {                                                           \
        if ((ptr) == NULL)                                      \
        {                                                       \
            fprintf(stderr,                                     \
                    "Error in memory allocation at %s:%s:%d\n", \
                    __FILE__, __FUNCTION__, __LINE__);          \
            exit(EXIT_FAILURE);                                 \
        }                                                       \
    } while (0)

enum
{
    SUCCESS = EXIT_SUCCESS,
    ERROR = EXIT_FAILURE
};
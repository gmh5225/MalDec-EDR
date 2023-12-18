#pragma once

#include <stdio.h>
#include <stdlib.h>

/**
 * @brief check cond ptr is NULL
 * 
 */
#define IS_NULL_PTR(ptr) ((ptr) == NULL)
#define NO_USE_AFTER_FREE(ptr) (ptr) = NULL;

/**
 * @brief Check if memory allocation was successful, and exit with an error message if not.
 *
 * This function checks if the provided pointer is NULL, indicating a failure in memory allocation.
 * If the pointer is NULL, an error message is printed to the standard error stream, indicating
 * the file, function, and line number where the allocation error occurred. The program then exits
 * with a failure status.
 *
 * @param ptr Pointer to the allocated memory.
 *
 * @note This function is intended to be used as a macro to quickly check and handle memory allocation errors.
 *
 * Example usage:
 * @code
 *   int *dynamicArray = (int*)malloc(size * sizeof(int));
 *   ALLOC_ERR(dynamicArray);
 * @endcode
 *
 * @param ptr Pointer to the allocated memory.
 */
static inline void ALLOC_ERR(const void *restrict ptr)
{
    if (IS_NULL_PTR(ptr))
    {
        fprintf(stderr,
                "Error in memory allocation at %s:%s:%d\n",
                __FILE__, __FUNCTION__, __LINE__);
        exit(EXIT_FAILURE);
    }
}
#pragma once

#include "ptr_err.h"

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

/**
 * @brief Enumeration for success and error values.
 */
enum
{
    SUCCESS = EXIT_SUCCESS, /**< Indicates successful operation. */
    ERROR = EXIT_FAILURE    /**< Indicates an error occurred. */
};

/**
 * @brief Macro to check if a value represents an error.
 *
 * This macro checks if the given value is equal to the error code.
 * @param x The value to check.
 * @return Returns true if the value represents an error, false otherwise.
 */
#define IS_ERR_VALUE(x) ((x) == ERROR)

/**
 * @brief Function to check if a value represents an error.
 *
 * This function checks if the given integer value represents an error.
 * @param err The value to check.
 * @return Returns true if the value represents an error, false otherwise.
 */
static inline bool IS_ERR(const int err)
{
    return IS_ERR_VALUE(err);
}
/**
 * @file cjson.h
 * @brief Header file for JSON handling functions.
 */

#pragma once

#include "cjson.h"
#include "err/err.h"

#include "json-c/arraylist.h"
#include "json-c/debug.h"
#include "json-c/json_c_version.h"
#include "json-c/json_object.h"
#include "json-c/json_object_iterator.h"
#include "json-c/json_patch.h"
#include "json-c/json_pointer.h"
#include "json-c/json_tokener.h"
#include "json-c/json_util.h"
#include "json-c/linkhash.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/**
 * @brief Initializes a JSON object from a file.
 *
 * This function reads a JSON file, parses its content, and initializes a JSON object.
 *
 * @param json_obj Pointer to a pointer that will be set to the initialized JSON object.
 * @param filename Name of the JSON file to be read.
 * @return Returns ERR_SUCCESS on success, or ERROR on failure.
 */
ERR
init_cjson(struct json_object **json_obj, const char *filename);

/**
 * @brief Frees resources associated with a JSON object.
 *
 * This function releases the memory allocated for a JSON object.
 *
 * @param[in,out] json_obj Pointer to the JSON object to be freed.
 */
void
exit_json(struct json_object **json_obj);
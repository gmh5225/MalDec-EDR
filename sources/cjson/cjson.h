#pragma once

#include <json-c/arraylist.h>
#include <json-c/json_c_version.h>
#include <json-c/json_object.h>
#include <json-c/json_object_iterator.h>
#include <json-c/json_patch.h>
#include <json-c/json_pointer.h>
#include <json-c/json_tokener.h>
#include <json-c/json_util.h>
#include <json-c/linkhash.h>

#include "compiler/compiler_attribute.h"

int init_json(struct json_object **json_obj, const char* filename) check_unused_result;
void exit_json(struct json_object **json_obj);
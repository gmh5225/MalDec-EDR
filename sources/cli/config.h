#pragma once

#include "logger/logger.h"
#include "scan/config.h"
#include "scan/scan.h"
#include "cjson/cjson.h"
#include "inotify/inotify.h"
#include "compiler/compiler_attribute.h"

struct packed(8) DEFENDER
{
	LOGGER *logger;
	SCANNER *scanner;
	struct json_object *config_json;
} DEFENDER;
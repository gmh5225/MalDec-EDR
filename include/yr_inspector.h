#pragma once

#include<yara/types.h>

#define DEFAULT_SCAN_CALLBACK default_scan_callback 

typedef struct INSPECTOR INSPECTOR;

int inspector_init(INSPECTOR **inspector);
int inspector_scan_file(INSPECTOR *inspector, const char *file, YR_CALLBACK_FUNC callback);
int default_scan_callback(YR_SCAN_CONTEXT *context, int message, void *message_data, void *user_data);

#pragma once

typedef struct QUARANTINE_FILES
{
  const char *filename;
  const char *filepath;
  const char *datatime;
  const char *detected;
} QUARANTINE_FILES;

typedef struct QUARANTINE_CONFIG
{
  const char *dir;

} QUARANTINE_CONFIG;

typedef struct INSPECTOR_CONFIG
{
  QUARANTINE_CONFIG quarantine;
  const char       *dir;
  const char       *database;

} INSPECTOR_CONFIG;
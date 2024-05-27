#pragma once

typedef struct QUARANTINE_FILE
{
  int         id;
  const char *filename;
  const char *filepath;
  const char *datetime;
  const char *detected;
} QUARANTINE_FILE;

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
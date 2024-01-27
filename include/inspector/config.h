#pragma once

typedef struct QUARANTINE_CONFIG
{
  const char *dir;

} QUARANTINE_CONFIG;

typedef struct INSPECTOR_CONFIG
{
  QUARANTINE_CONFIG quarantine;
  const char       *dir;
  const char       *path;

} INSPECTOR_CONFIG;
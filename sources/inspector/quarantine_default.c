#include "inspector/inspector.h"
#include <stdio.h>

#define COLUMN_WIDTH 20

inline int
default_view_quarantine(void *unused, const int count, char **data,
                        char **columns)
{
  unused = unused;

  for (int i = 0; i < count; i++)
  {
    fprintf(stdout, "| %-*s : %-*s\n", COLUMN_WIDTH - 2, columns[i],
            COLUMN_WIDTH - 2, data[i]);
  }

  return 0;
}
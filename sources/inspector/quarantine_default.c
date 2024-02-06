#include "inspector/inspector.h"
#include <stdio.h>

#define COLUMN_WIDTH 20

// Function to display a single row of column names and data
static inline void
print_row(const int count, char **columns, char **data)
{
  for (int i = 0; i < count; i++)
  {
    fprintf(stdout, "| %-*s : %-*s\n", COLUMN_WIDTH - 2, columns[i],
            COLUMN_WIDTH - 2, data[i]);
  }
}

// Main function for default view in quarantine
inline int
default_view_quarantine(void *unused, const int count, char **data,
                        char **columns)
{
  unused = unused;

  // Display column names and data
  print_row(count, columns, data);

  return 0;
}
#include "inspector/inspector.h"

#define COLUMN_WIDTH 25

static inline void
print_line(const int count, const int width)
{
  for (int i = 0; i < count; i++)
  {
    fprintf(stdout, "+%*s", width, "---------------------------");
  }
  fprintf(stdout, "+\n");
}

inline int
default_view_quarantine(void *unused, const int count, char **data,
                        char **columns)
{
  unused = unused;

  // Imprime cabeçalho da tabela
  print_line(count, COLUMN_WIDTH);
  for (int idx = 0; idx < count; idx++)
  {
    fprintf(stdout, "| %-*s", COLUMN_WIDTH, columns[idx]);
  }
  fprintf(stdout, "|\n");

  // Imprime separador entre cabeçalho e dados
  print_line(count, COLUMN_WIDTH);

  // Imprime os dados
  for (int i = 0; i < count; i++)
  {
    fprintf(stdout, "| %-*s", COLUMN_WIDTH, data[i]);
  }
  fprintf(stdout, "|\n");

  // Imprime rodapé da tabela
  print_line(count, COLUMN_WIDTH);

  return 0;
}

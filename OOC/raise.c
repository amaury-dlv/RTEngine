#include "OOC.h"
#include "raise.h"

void		raise_int(char *const file, int line, char *msg, ...)
{
  va_list	list;

  va_start(list, msg);
  fprintf(stderr, "%s: %u: ", file, line);
  vfprintf(stderr, msg, list);
  va_end(list);
  abort();
}

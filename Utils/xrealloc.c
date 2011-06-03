#include <stdlib.h>
#include <unistd.h>

#include "OOC.h"

void		*xrealloc(void *ptr, int size)
{
  ptr = realloc(ptr, size);
  if (ptr == NULL)
    raise("Out of memory\n");
  return (ptr);
}

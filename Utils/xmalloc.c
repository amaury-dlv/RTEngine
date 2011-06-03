#include <stdlib.h>
#include <unistd.h>

#include "OOC.h"

void		*xmalloc(int size)
{
  void		*ptr;

  ptr = malloc(size);
  if (ptr == NULL)
    raise("Out of memory\n");
  return (ptr);
}

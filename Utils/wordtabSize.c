#include "Utils.h"

int		wordtabSize(char **wtab)
{
  int		size;

  size = 0;
  while (wtab[size])
    size++;
  return (size);
}

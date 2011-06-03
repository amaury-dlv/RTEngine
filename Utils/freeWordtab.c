#include <stdlib.h>

#include "Utils.h"

void		freeWordtab(char **tab)
{
  int		i;

  i = 0;
  while (tab[i])
    free(tab[i++]);
  free(tab);
}

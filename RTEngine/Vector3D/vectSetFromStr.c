#include <math.h>

#include "Vector3D.h"
#include "Utils.h"

int		vectSetFromStr(Vector3D *self, char *str)
{
  char			**tab;

  tab = toWordtab(str, " ,");
  if (!tab || wordtabSize(tab) != 3)
    return (-1);
  if (setDoubleFromStr(tab[0], &self->x) != 0
      || setDoubleFromStr(tab[1], &self->y) != 0
      || setDoubleFromStr(tab[2], &self->z) != 0)
    return (-1);
  freeWordtab(tab);
  return (0);
}


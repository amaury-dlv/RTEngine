#include "Color.h"
#include "Maths.h"
#include "Utils.h"

int		setColorFromStr(char *str, Color *color)
{
  char			**tab;

  tab = toWordtab(str, " ,");
  if (!tab || wordtabSize(tab) != 3)
    return (-1);
  if (setFloatFromStr(tab[0], &color->r) != 0
      || setFloatFromStr(tab[1], &color->g) != 0
      || setFloatFromStr(tab[2], &color->b) != 0)
    return (-1);
  freeWordtab(tab);
  return (0);
}

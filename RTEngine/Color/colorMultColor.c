#include "Color.h"
#include "Maths.h"
#include "Utils.h"

Color		*colorMultColor(Color *color, Color *k)
{
  color->r *= k->r;
  color->g *= k->g;
  color->b *= k->b;
  return (color);
}


#include "Color.h"
#include "Maths.h"
#include "Utils.h"

Color		*colorMult(Color *color, float k)
{
  color->r *= k;
  color->g *= k;
  color->b *= k;
  return (color);
}


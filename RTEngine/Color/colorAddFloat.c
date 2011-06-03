#include "Color.h"
#include "Maths.h"
#include "Utils.h"

Color		*colorAddFloat(Color *color, float k)
{
  color->r += k;
  color->g += k;
  color->b += k;
  return (color);
}

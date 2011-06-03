#include "Color.h"
#include "Maths.h"
#include "Utils.h"

Color		colorGetClamped(Color *color)
{
  Color		ret;
  float		k;

  ret = *color;
  k = MAX(ret.r, MAX(ret.g, ret.b));
  if (k > 1.f)
    colorMult(&ret, 1.f / k);
  return (ret);
}


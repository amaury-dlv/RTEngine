#include "Color.h"
#include "Maths.h"
#include "Utils.h"

unsigned int	colorGetUInt(Color* color)
{
  unsigned int	r;
  unsigned int	g;
  unsigned int	b;
  Color		tmp;

  tmp = *color;
  r =  MIN(1.f, tmp.r) * 255.f;
  g =  MIN(1.f, tmp.g) * 255.f;
  b =  MIN(1.f, tmp.b) * 255.f;
  return (0xff000000 | (r << 16) | (g << 8) | (b << 0));
}

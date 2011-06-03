#include "Maths.h"

double	clamp(double x, double min, double max)
{
  return (x < min ? min : (x > max ? max : x));
}

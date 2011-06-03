#include <math.h>

#include "PerlinNoise.h"

double fade(double t)
{
  return (t * t * t * (t * (t * 6 - 15) + 10));
}

double lerp(double t, double a, double b)
{
  return (a + t * (b - a));
}

double grad(int hash, double x, double y, double z)
{
  int     h;
  double  u;
  double  v;

  h = hash & 15;
  u = h < 8 ? x : y;
  v = h < 4 ? y : h == 12 || h == 14 ? x : z;
  return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
}

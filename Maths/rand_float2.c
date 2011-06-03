#include "Maths.h"

float	rand_float2(int l, float h)
{
  return (rand_float() * (h - l) + l);
}

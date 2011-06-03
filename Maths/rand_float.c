#include "Maths.h"

static const float 	invRAND_MAX = 1.0 / (float)RAND_MAX;

float	rand_float(void)
{
  return ((float) rand_int() * invRAND_MAX);
}

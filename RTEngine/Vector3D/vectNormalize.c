#include <math.h>

#include "Vector3D.h"
#include "Utils.h"

void	vectNormalize(Vector3D* self)
{
  double	length_inv;

  length_inv = self->x * self->x + self->y * self->y + self->z * self->z;
  if (length_inv != 0.0)
  {
    length_inv = 1.0 / sqrt(length_inv);
    self->x *= length_inv;
    self->y *= length_inv;
    self->z *= length_inv;
  }
}


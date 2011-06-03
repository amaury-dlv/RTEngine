#include <math.h>

#include "Vector3D.h"
#include "Utils.h"

Vector3D	*vectInv(Vector3D *self)
{
  self->x = -self->x;
  self->y = -self->y;
  self->z = -self->z;
  return (self);
}


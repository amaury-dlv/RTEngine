#include <math.h>

#include "Vector3D.h"
#include "Utils.h"

Vector3D	*vector3DSet(Vector3D *self, double x, double y, double z)
{
  self->x = x;
  self->y = y;
  self->z = z;
  return (self);
}


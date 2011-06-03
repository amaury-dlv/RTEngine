#include <math.h>

#include "Vector3D.h"
#include "Utils.h"

Vector3D*	vectCrossProduct(Vector3D* self, Vector3D* other)
{
  double		tmpX;
  double		tmpY;
  double		tmpZ;

  tmpX = (self->y * other->z) - (self->z * other->y);
  tmpY = (self->z * other->x) - (self->x * other->z);
  tmpZ = (self->x * other->y) - (self->y * other->x);
  self->x = tmpX;
  self->y = tmpY;
  self->z = tmpZ;
  return (self);
}


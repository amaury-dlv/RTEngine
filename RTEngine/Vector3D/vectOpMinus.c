#include <math.h>

#include "Vector3D.h"
#include "Utils.h"

Vector3D*	vectOpMinus(Vector3D* self, Vector3D *other)
{
  self->x -= other->x;
  self->y -= other->y;
  self->z -= other->z;
  return (self);
}


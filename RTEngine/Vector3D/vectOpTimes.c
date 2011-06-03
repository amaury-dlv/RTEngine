#include <math.h>

#include "Vector3D.h"
#include "Utils.h"

Vector3D*	vectOpTimes(Vector3D* self, double a)
{
  self->x *= a;
  self->y *= a;
  self->z *= a;
  return (self);
}

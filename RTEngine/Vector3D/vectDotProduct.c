#include <math.h>

#include "Vector3D.h"
#include "Utils.h"

double		vectDotProduct(Vector3D* self, Vector3D *other)
{
  return (self->x * other->x
          + self->y * other->y
          + self->z * other->z);
}

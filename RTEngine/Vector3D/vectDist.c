#include <math.h>

#include "Vector3D.h"
#include "Utils.h"

double		vectDist(Vector3D* self, Vector3D* other)
{
  double		dist;

  dist = sqrt((self->x - other->x) * (self->x - other->x)
            + (self->y - other->y) * (self->y - other->y)
            + (self->z - other->z) * (self->z - other->z));
  return (dist);
}


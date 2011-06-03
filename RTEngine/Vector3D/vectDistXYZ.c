#include <math.h>

#include "Vector3D.h"
#include "Utils.h"

double		vectDistXYZ(Vector3D *self, double x, double y, double z)
{
  double		dist;

  dist = sqrt((self->x - x) * (self->x - x)
      + (self->y - y) * (self->y - y)
      + (self->z - z) * (self->z - z));
  return (dist);
}


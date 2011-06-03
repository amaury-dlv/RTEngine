#include <math.h>

#include "Vector3D.h"
#include "Utils.h"

double	vectLength(Vector3D* self)
{
  return (sqrt(self->x * self->x
               + self->y * self->y
               + self->z * self->z));
}


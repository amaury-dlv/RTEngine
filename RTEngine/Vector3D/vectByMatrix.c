#include <math.h>

#include "Vector3D.h"
#include "Utils.h"

Vector3D	*vectByMatrix(Vector3D *self, double m[4][4])
{
  Vector3D tmp;

  tmp.x = self->x;
  tmp.y = self->y;
  tmp.z = self->z;
  self->x = m[0][0] * tmp.x + m[0][1] * tmp.y + m[0][2] * tmp.z;
  self->y= m[1][0] * tmp.x + m[1][1] * tmp.y + m[1][2] * tmp.z;
  self->z= m[1][0] * tmp.x + m[1][1] * tmp.y + m[1][2] * tmp.z;
  return (self);
}

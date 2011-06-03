#include "Maths.h"

void	toRad(Point3D *mRot)
{
  mRot->x *= Pi / 180.;
  mRot->y *= Pi / 180.;
  mRot->z *= Pi / 180.;
}


#include <math.h>

#include "Vector3D.h"
#include "Utils.h"

Vector3D		vectReflect(Vector3D *i, Vector3D *n)
{
  Vector3D	ref;

  ref = *n;
  vectOpTimes(&ref, 2.0 * vectDotProduct(i, n));
  vectInv(&ref);
  vectOpPlus(&ref, i);
  return (ref);
}


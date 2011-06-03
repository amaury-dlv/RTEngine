#include "Ray.h"
#include "Maths.h"

bool		rayIsValid(RayStruct *ray)
{
  return (ray->direction.x != DeathNumber);
}

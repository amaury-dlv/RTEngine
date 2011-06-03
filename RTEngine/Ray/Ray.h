#ifndef __RAY_H__
# define __RAY_H__

# include "OOC.h"
# include "Point3D.h"
# include "Vector3D.h"

typedef struct RayStruct	RayStruct;
struct				RayStruct
{
  Point3D			origin;
  Vector3D			direction;
};

bool			rayIsValid(RayStruct *ray);

#endif /* !__RAY_H__ */

#ifndef __RECTANGLE_H__
# define __RECTANGLE_H__

# include "GeometricObject.h"
# include "Vector3D.h"
# include "Point3D.h"
# include "SceneSerializer.h"

typedef struct RectangleClass	RectangleClass;
struct                          RectangleClass
{
  /* SuperClass : */
  GeometricObjectClass		__base__;
  /* public : */
  /* nothing yet */
  /* private : */
  struct
  {
    Vector3D			u;
    Vector3D			v;
    Vector3D			n;
    double			lenU;
    double			lenV;
  } private;
};

extern Class*			Rectangle;

static const double		RectangleKEpsilon = 0.01;

void	*rectangleUnserialize();

#endif /* !__RECTANGLE_H__ */

#ifndef		__PLANE_H__
# define	__PLANE_H__

# include "GeometricObject.h"
# include "Vector3D.h"
# include "Point3D.h"

typedef struct PlaneClass	PlaneClass;
struct                          PlaneClass
{
  /* SuperClass : */
  GeometricObjectClass		__base__;
  /* public : */
  /* nothing yet */
  /* private : */
  void				(*setNormal)(PlaneClass *self,
                                             double x, double y, double z);
  struct
  {
    Vector3D		mNormal;
  } private;
};

extern Class*			Plane;

static const double		PlaneKEpsilon = 0.001;

void		*planeUnserialize();

#endif		/* ! __PLANE_H__ */

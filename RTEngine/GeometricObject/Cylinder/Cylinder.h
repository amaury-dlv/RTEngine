#ifndef __CYLINDER_H__
# define __CYLINDER_H__

# include "GeometricObject.h"
# include "Vector3D.h"
# include "Point3D.h"

typedef struct CylinderClass	CylinderClass;
struct                          CylinderClass
{
  /* SuperClass : */
  GeometricObjectClass		__base__;
  /* public : */
  /* nothing yet */
  /* private : */
  struct
  {
    double			mHighY;
    double			mLowY;
    float			mRadius;
  } private;
};

extern Class*			Cylinder;

void		*cylinderUnserialize();

#endif /* !__CYLINDER_H__ */

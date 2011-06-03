#ifndef __CONE_H__
# define __CONE_H__

# include "GeometricObject.h"
# include "Vector3D.h"
# include "Point3D.h"

typedef struct ConeClass	ConeClass;
struct                          ConeClass
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
    float			mAlphaAngle;
  } private;
};

extern Class*			Cone;

void		*coneUnserialize();

#endif /* !__CONE_H__ */

#ifndef __SPHERICAL_H__
# define __SPHERICAL_H__

# include "Camera.h"

# define SPHERICAL(x)		((SphericalClass*) (x))

typedef struct SphericalClass	SphericalClass;
struct                          SphericalClass
{
  /* SuperClass : */
  CameraClass			__base__;
  /* public : */
  /* private : */
  struct
  {
    float			mPsi;
    float			mLambda;
  } private;
};

extern Class*			Spherical;

void		*sphericalUnserialize();

#endif /* !__SPHERICAL_H__ */

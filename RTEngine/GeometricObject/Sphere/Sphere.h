#ifndef		__SPHERE_H__
# define	__SPHERE_H__

# include "GeometricObject.h"
# include "Vector3D.h"
# include "Point3D.h"
# include "SceneSerializer.h"

typedef struct SphereClass	SphereClass;
struct                          SphereClass
{
  /* SuperClass : */
  GeometricObjectClass		__base__;
  /* public : */
  /* nothing yet */
  /* private : */
  struct
  {
    float			mRadius;
    int				mIsConcave;
  } private;
};

extern Class*			Sphere;

static const double		SphereKEpsilon = 0.01;

void	*sphereUnserialize();

#endif		/* ! __SPHERE_H__ */

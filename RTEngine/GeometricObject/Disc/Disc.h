#ifndef		__DISC_H__
# define	__DISC_H__

# include "GeometricObject.h"
# include "Vector3D.h"
# include "Point3D.h"

typedef struct DiscClass	DiscClass;
struct                          DiscClass
{
  /* SuperClass : */
  GeometricObjectClass		__base__;
  /* public : */
  /* nothing yet */
  /* private : */
  void				(*setNormal)(DiscClass *self,
					     double x,
					     double y,
					     double z);
  struct
  {
    Vector3D		mNormal;
    float		mRadius;
  } private;
};

extern Class*			Disc;

static const double		DiscKEpsilon = 0.001;

void		*discUnserialize();

#endif		/* ! __DISC_H__ */

#ifndef __TORE_H__
# define __TORE_H__

# include "GeometricObject.h"
# include "Vector3D.h"
# include "Point3D.h"

# define	SPN	(0.01)

typedef struct ToreInter	ToreInter;
struct				ToreInter
{
  Vector3D			v;
  Vector3D			w;
  Vector3D			t;
  double			four_a_sqrd;
  double			sum_d_sqrd;
  double			e;
  double			f;
  double			coeffs[5];
  double			tradius;
};

typedef struct ToreClass	ToreClass;
struct                          ToreClass
{
  /* SuperClass : */
  GeometricObjectClass		__base__;
  /* public : */
  /* nothing yet */
  /* private : */
  struct
  {
    float			mRadius; /* Tore Radius */
    float			mT;
  } private;
};

extern Class*			Tore;

void				*toreUnserialize();

#endif /* !__TORE_H__ */

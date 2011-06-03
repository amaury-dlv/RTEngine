#ifndef __CUBETROUE_H__
# define __CUBETROUE_H__

# include "GeometricObject.h"
# include "Vector3D.h"
# include "Point3D.h"

typedef struct CubeTroueInter	CubeTroueInter;
struct				CubeTroueInter
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

typedef struct CubeTroueClass	CubeTroueClass;
struct                          CubeTroueClass
{
  /* SuperClass : */
  GeometricObjectClass		__base__;
  /* public : */
  /* nothing yet */
  /* private : */
  struct
  {
    float			hole;
    float			size;
    float			alpha;
  } private;
};

extern Class*			CubeTroue;

void				*cubeTroueUnserialize();

#endif /* !__CUBETROUE_H__ */

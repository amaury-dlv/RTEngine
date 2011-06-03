#ifndef __BOX_H__
# define __BOX_H__

# include "GeometricObject.h"
# include "Vector3D.h"
# include "Point3D.h"

typedef struct BoxInter		BoxInter;
struct				BoxInter
{
    float			t0;
    float			t1;
    float			txMin;
    float			tyMin;
    float			tzMin;
    float			txMax;
    float			tyMax;
    float			tzMax;
    int				faceIn;
    int				faceOut;
    float			rox;
    float			roy;
    float			roz;
    float			rdx;
    float			rdy;
    float			rdz;
    float			a;
    float			b;
    float			c;
};

typedef struct BoxClass		BoxClass;
struct                          BoxClass
{
  /* SuperClass : */
  GeometricObjectClass		__base__;
  /* public : */
  /* nothing yet */
  /* private : */
  struct
  {
    float			x0;
    float			y0;
    float			z0;
    float			x1;
    float			y1;
    float			z1;
  } private;
};

extern Class*			Box;

void		*boxUnserialize();

#endif /* !__BOX_H__ */

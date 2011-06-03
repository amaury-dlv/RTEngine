#ifndef __PINHOLE_H__
# define __PINHOLE_H__

# include "Camera.h"

# define PINHOLE(x)		((PinholeClass*) (x))

typedef struct PinholeClass	PinholeClass;
struct                          PinholeClass
{
  /* SuperClass : */
  CameraClass			__base__;
  /* public : */
  void				(*setViewDist)(PinholeClass* self, float dist);
  /* private : */
  struct
  {
    float			mViewDist;
  } private;
};

extern Class*			Pinhole;

void		*pinholeUnserialize();

#endif /* !__PINHOLE_H__ */

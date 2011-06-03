#ifndef __FAKESPHERICALLIGHT_H__
# define __FAKESPHERICALLIGHT_H__

# include "Light.h"

typedef struct FakeSphericalLightClass	FakeSphericalLightClass;
struct                          FakeSphericalLightClass
{
  /* SuperClass : */
  LightClass			__base__;
  /* private: */
  void				(*setPosition)(FakeSphericalLightClass *self,
                                               double x, double y, double z);
  Point3D			mPos;
};

extern Class*			FakeSphericalLight;

void		*fakeSphericalLightUnserialize();

#endif /* !__FAKESPHERICALLIGHT_H__ */

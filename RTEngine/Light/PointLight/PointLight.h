#ifndef __POINTLIGHT_H__
# define __POINTLIGHT_H__

# include "Light.h"
# include "Point3D.h"

# define POINTLIGHT(x)		((PointLightClass*) (x))

typedef struct PointLightClass	PointLightClass;
struct                          PointLightClass
{
  /* SuperClass : */
  LightClass			__base__;
  /* private: */
  void				(*setPosition)(PointLightClass *self,
                                               double x, double y, double z);
  Point3D			mPos;
};

extern Class*			PointLight;

void		*pointLightUnserialize();

#endif /* !__POINTLIGHT_H__ */

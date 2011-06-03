#ifndef __DIRECTIONAL_H__
# define __DIRECTIONAL_H__

# include "Light.h"

# define DIRECTIONAL(x)		((DirectionalClass*) (x))

typedef struct DirectionalClass	DirectionalClass;
struct                          DirectionalClass
{
  /* SuperClass : */
  LightClass			__base__;
  void				(*setDirection)();
  /* private: */
  struct
  {
    Vector3D		mDir;
  } private;
};

extern Class*			Directional;

void		*directionalUnserialize();

#endif /* !__DIRECTIONAL_H__ */

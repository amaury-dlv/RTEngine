#ifndef __LIGHT_H__
# define __LIGHT_H__

# include "OOC.h"
# include "Color.h"
# include "Vector3D.h"
# include "Ray.h"

# define LIGHT(x)		((LightClass*) (x))

typedef struct LightClass	LightClass;
struct                          LightClass
{
  /* SuperClass : */
  Class			__base__;
  /* public: */
  Vector3D		(*getDirection)();
  Color			(*l)();
  bool			(*inShadow)(LightClass *self, RayStruct *shadowRay);
  void			(*setHitRecordDist)();
  /* public : */
  int			id;
  float			ls;
  Color			color;
};

extern Class*			Light;

int		lightUnserializeSet();

#endif /* !__LIGHT_H__ */

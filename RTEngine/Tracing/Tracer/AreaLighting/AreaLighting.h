#ifndef __AREALIGHTING_H__
# define __AREALIGHTING_H__

# include "Tracer.h"
# include "HitRecord.h"
# include "Color.h"

# define	AREALIGHTING(x)	((AreaLightingClass*) (x))

typedef struct AreaLightingClass	AreaLightingClass;
struct                          	AreaLightingClass
{
  /* SuperClass : */
  TracerClass				__base__;
  /* public : */
  Color					(*trace)(AreaLightingClass* self, RayStruct* ray);
  /* public : */
  /* protected : */
};

extern Class*				AreaLighting;

#endif /* !__AREALIGHTING_H__ */

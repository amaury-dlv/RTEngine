#ifndef __LAMBERTIAN_H__
# define __LAMBERTIAN_H__

# include "OOC.h"
# include "BRDF.h"
# include "Color.h"
# include "Maths.h"

# define	LAMBERTIAN(x)	((LambertianClass *)(x))

typedef struct LambertianClass	LambertianClass;
struct                          LambertianClass
{
  /* SuperClass : */
  BRDFClass			__base__;
  void				(*setKd)(LambertianClass *self, float kd);
  /* private : */
  struct
  {
    bool			mAttenuation;
    float			mKd;
  } private;
};

extern Class*			Lambertian;

#endif /* !__LAMBERTIAN_H__ */

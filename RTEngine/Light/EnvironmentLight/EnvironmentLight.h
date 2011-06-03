#ifndef __ENVIRONMENTLIGHT_H__
# define __ENVIRONMENTLIGHT_H__

# include "Light.h"
# include "Point3D.h"
# include "Sampler.h"
# include "Material.h"

# define ENVLIGHT(x)		((EnvironmentLightClass*) (x))

typedef struct EnvironmentLightClass	EnvironmentLightClass;
struct                          	EnvironmentLightClass
{
  /* SuperClass : */
  LightClass			__base__;
  /* public: */
  /* private: */
  float				(*g)();
  float				(*pdf)();
  int				samples;
  struct /* private : */
  {
    SamplerClass		*mSampler;
    MaterialClass		*mMaterial;
    Vector3D		u;
    Vector3D		v;
    Vector3D		w;
    Vector3D		wi;
  } private;
};

extern Class*			EnvironmentLight;

void		*environmentLightUnserialize();

#endif /* !__ENVIRONMENTLIGHT_H__ */

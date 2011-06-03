#ifndef __AMBIENT_H__
# define __AMBIENT_H__

# include "Light.h"
# include "bool.h"
# include "Sampler.h"

# define AMBIENT(x)		((AmbientClass*) (x))

typedef struct AmbientClass	AmbientClass;
struct                          AmbientClass
{
  /* SuperClass : */
  LightClass			__base__;
  /* public : */
  void				(*setOccluder)(AmbientClass *self,
                                               int samples, float occlusion);
  struct /* private : */
  {
    bool			mIsOccluder;
    Vector3D			u;
    Vector3D			v;
    Vector3D			w;
    SamplerClass		*mSampler;
    int				mSamples;
    float			mOcclusionFactor;
  } private;
};

extern Class*			Ambient;

void		*ambientUnserialize();

#endif /* !__AMBIENT_H__ */

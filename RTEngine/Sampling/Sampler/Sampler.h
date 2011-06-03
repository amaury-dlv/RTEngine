#ifndef __SAMPLER_H__
# define __SAMPLER_H__

# include "OOC.h"
# include "RTTI.h"
# include "Point2D.h"
# include "Vector3D.h"

# define	SAMPLER(x)	((SamplerClass*) (x))

typedef struct SamplerClass	SamplerClass;
struct                          SamplerClass
{
  /* SuperClass : */
  Class				__base__;
  /* public : */
  void				(*generateSamples)(SamplerClass *self);
  int				(*getNumSamples)(SamplerClass *self);
  int				(*getNumSets)(SamplerClass *self);
  float				(*getE)(SamplerClass *self);
  void				(*generateSample)();
  void				(*setupShuffledIndices)(SamplerClass *self);
  Point2D			(*nextSample)(SamplerClass *self);
  Vector3D			(*nextHemisphereSample)(SamplerClass *self);
  Vector3D			(*nextSphereSample)(SamplerClass *self);
  void				(*mapSamplesToUnitDisk)(SamplerClass *self);
  void				(*mapSamplesToHemisphere)(SamplerClass *self,
    float e);
  void				(*mapSamplesToSphere)(SamplerClass *self);
  void				(*shuffleX)(SamplerClass *self);
  void				(*shuffleY)(SamplerClass *self);
  /* public : */
  /* protected : */
  struct
  {
    int				mNumSamples;
    int				mSqrtNumSamples;
    int				mNumSets;
    Point2D			*mSamples;
    int				*mShuffledIndices;
    Point2D       		*mDiskSamples;
    Vector3D			*mHemisphereSamples;
    Vector3D			*mSphereSamples;
    unsigned int		mCount;
    int				mJump;
    float			mE;
    int				mThread[16];
  } protected;
};

extern Class*			Sampler;

extern void			setSamplerNumSamples(int numSamples);
void				samplerInit(SamplerClass *self, va_list *list);

#endif /* !__SAMPLER_H__ */

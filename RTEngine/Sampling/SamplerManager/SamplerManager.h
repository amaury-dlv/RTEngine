#ifndef __SAMPLERMANAGER_H__
# define __SAMPLERMANAGER_H__

# include "OOC.h"
# include "RTTI.h"
# include "Vector.h"
# include "Sampler.h"
# include "Regular.h"
# include "Jittered.h"
# include "PureRandom.h"
# include "MultiJittered.h"

typedef enum			SamplerType
{
  eRegular = __RTTI_REGULAR,
  eJittered = __RTTI_JITTERED,
  ePureRandom = __RTTI_PURERANDOM,
  eMultiJittered = __RTTI_MULTIJITTERED
}				SamplerType;

typedef struct SamplerManagerClass	SamplerManagerClass;
struct                          	SamplerManagerClass
{
  /* SuperClass : */
  Class				__base__;
  /* public : */
  SamplerClass*			(*getSampler)(SamplerType type, int numSamples,
                                              int numSets, float e);
  /* private : */
  struct
  {
    VectorClass			*mSamplerVect;
  } private;
};

SamplerManagerClass* 		SamplerManagerGetSingletonPtr(void);

extern Class* 			SamplerManager;

#endif /* !__SAMPLERMANAGER_H__ */

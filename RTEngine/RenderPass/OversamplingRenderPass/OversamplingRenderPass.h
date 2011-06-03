#ifndef __OVERSAMPLINGRENDERPASS_H__
# define __OVERSAMPLINGRENDERPASS_H__

# include "OOC.h"
# include "RenderPass.h"
# include "Raycast.h"
# include "HitRecord.h"
# include "Sampler.h"

typedef struct OversamplingRenderPassClass	OversamplingRenderPassClass;
struct                          		OversamplingRenderPassClass
{
  /* SuperClass : */
  RenderPassClass		__base__;
  /* public : */
  /* private : */
  struct
  {
    ColorBufferClass		*mBuffer;
    RaycastClass		*mTracer;
    int				mNumSamples;
    SamplerClass		*mSampler;
  } private;
};

extern Class*			OversamplingRenderPass;

void		*oversamplingRenderPassUnserialize();

#endif /* !__OVERSAMPLINGRENDERPASS_H__ */

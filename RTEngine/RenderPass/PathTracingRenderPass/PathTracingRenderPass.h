#ifndef __PATHTRACINGRENDERPASS_H__
# define __PATHTRACINGRENDERPASS_H__

# include "OOC.h"
# include "RenderPass.h"
# include "ColorBuffer.h"
# include "MaterialBuffer.h"
# include "Raycast.h"
# include "Sampler.h"

typedef struct PathTracingRenderPassClass	PathTracingRenderPassClass;
struct                          		PathTracingRenderPassClass
{
  /* SuperClass : */
  RenderPassClass		__base__;
  /* public : */
  /* private : */
  struct
  {
    MaterialBufferClass		*mMatBuffer;
    ColorBufferClass		*mCurrentState;
    int				mNumSamples;
    SamplerClass		*mSampler;
    RaycastClass		*mTracer;
  } private;
};

extern Class*			PathTracingRenderPass;

void		*pathTracingRenderPassUnserialize();

#endif /* !__PATHTRACINGRENDERPASS_H__ */

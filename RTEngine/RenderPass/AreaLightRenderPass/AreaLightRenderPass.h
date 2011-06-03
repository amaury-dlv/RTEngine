#ifndef __AREALIGHTRENDERPASS_H__
# define __AREALIGHTRENDERPASS_H__

# include "OOC.h"
# include "RenderPass.h"
# include "ColorBuffer.h"
# include "MaterialBuffer.h"
# include "Raycast.h"

typedef struct AreaLightRenderPassClass	AreaLightRenderPassClass;
struct                          		AreaLightRenderPassClass
{
  /* SuperClass : */
  RenderPassClass		__base__;
  /* public : */
  /* private : */
  struct
  {
    MaterialBufferClass		*mMatBuffer;
    ColorBufferClass		*mLightBuffer;
    ColorBufferClass		*mCurrentState;
    RaycastClass		*mTracer;
    SamplerClass		*mSampler;
    int				mSamples;
  } private;
};

extern Class*			AreaLightRenderPass;

void		*areaLightRenderPassUnserialize();

#endif /* !__AREALIGHTRENDERPASS_H__ */

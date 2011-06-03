#ifndef __AORENDERPASS_H__
# define __AORENDERPASS_H__

# include "OOC.h"
# include "RenderPass.h"
# include "LightBuffer.h"
# include "MaterialBuffer.h"
# include "Light.h"

# define	AORPASS(x)	((AORenderPassClass *)(x))

typedef struct AORenderPassClass	AORenderPassClass;
struct                          	AORenderPassClass
{
  /* SuperClass : */
  RenderPassClass		__base__;
  /* public : */
  /* private : */
  struct
  {
    LightClass			*curLight;
    LightBufferClass		*curLightBuf;
    MaterialBufferClass		*materialBuf;
    bool			mHasBlur;
    int				mSamples;
    float			mOcclusionFactor;
  } private;
};

extern Class*			AORenderPass;

void	*AORenderPassUnserialize();

#endif /* !__AORENDERPASS_H__ */

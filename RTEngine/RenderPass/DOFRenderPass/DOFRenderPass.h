#ifndef __DEPTHOFFIELDRENDERPASS_H__
# define __DEPTHOFFIELDRENDERPASS_H__

# include "OOC.h"
# include "RenderPass.h"
# include "ColorBuffer.h"
# include "DepthBuffer.h"

# define DOFRENDERPASS(x)	((DOFRenderPassClass*) (x))

typedef struct DOFRenderPassClass	DOFRenderPassClass;
struct                          	DOFRenderPassClass
{
  /* SuperClass : */
  RenderPassClass		__base__;
  /* public : */
  /* private : */
  struct
  {
    ColorBufferClass		*mColorBlurBuffer;
    DepthBufferClass		*mDepthBuffer;
    ColorBufferClass		*mCurrentState;
  } private;
};

extern Class*			DOFRenderPass;

#endif /* !__DEPTHOFFIELDRENDERPASS_H__ */

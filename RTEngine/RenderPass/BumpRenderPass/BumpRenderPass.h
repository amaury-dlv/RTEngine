#ifndef __BUMPRENDERPASS_H__
# define __BUMPRENDERPASS_H__

# include "OOC.h"
# include "RenderPass.h"
# include "ColorBuffer.h"
# include "MaterialBuffer.h"

# define BUMPRPASS(x)	((BumpRenderPassClass*) (x))

typedef struct BumpRenderPassClass	BumpRenderPassClass;
struct                          	BumpRenderPassClass
{
  /* SuperClass : */
  RenderPassClass		__base__;
  /* public : */
  /* private : */
  struct
  {
    MaterialBufferClass		*mMatBuffer;
    NormalBufferClass		*mNormalBuffer;
    ColorBufferClass		*mBumpBuffer;
    ColorBufferClass		*mCurrentState;
  } private;
};

extern Class*			BumpRenderPass;

void		*bumpRenderPassUnserialize();

#endif /* !__BUMPRENDERPASS_H__ */

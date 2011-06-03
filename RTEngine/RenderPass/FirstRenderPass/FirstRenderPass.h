#ifndef __FIRSTRENDERPASS_H__
# define __FIRSTRENDERPASS_H__

# include "OOC.h"
# include "RenderPass.h"
# include "Raycast.h"
# include "HitRecord.h"
# include "ColorBuffer.h"
# include "DepthBuffer.h"
# include "PointBuffer.h"
# include "NormalBuffer.h"
# include "MaterialBuffer.h"
# include "RayBuffer.h"
# include "BoolBuffer.h"

# define FIRSTRPASS(x)	((FirstRenderPassClass*) (x))

typedef struct FirstRenderPassClass	FirstRenderPassClass;
struct                          	FirstRenderPassClass
{
  /* SuperClass : */
  RenderPassClass		__base__;
  /* public : */
  /* private : */
  struct
  {
    RaycastClass		*mTracer;
    Color			mBGColor;
    /* buffers from GBuffer */
    ColorBufferClass		*colorBuffer;
    NormalBufferClass		*normalBuffer;
    RayBufferClass		*rayBuffer;
    DepthBufferClass		*depthBuffer;
    PointBufferClass		*pointBuffer;
    PointBufferClass		*localPointBuffer;
    MaterialBufferClass		*materialBuffer;
    BoolBufferClass		*insideBuffer;
  } private;
};

extern Class*			FirstRenderPass;

#endif /* !__FIRSTRENDERPASS_H__ */

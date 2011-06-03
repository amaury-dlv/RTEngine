#ifndef __SSAORENDERPASS_H__
# define __SSAORENDERPASS_H__

# include "OOC.h"
# include "RenderPass.h"
# include "LightRenderPass.h"
# include "Sampler.h"
# include "Point2D.h"
# include "DepthBuffer.h"

# define	SSAORPASS(x)	((SSAORenderPassClass *)(x))

typedef struct SSAORenderPassClass	SSAORenderPassClass;
struct                          	SSAORenderPassClass
{
  /* SuperClass : */
  RenderPassClass		__base__;
  /* public : */
  /* private : */
  struct
  {
    float			totStrength;
    float			strength;
    float			falloff;
    float			rad;
    int				samples;
    int				blurKernelSize;
    SamplerClass		*sphereSampler;
    SamplerClass		*normalSampler;
    LightClass			*light;
    LightBufferClass		*lightBuf;
    PointBufferClass		*pointBuf;
    NormalBufferClass		*normalBuf;
    DepthBufferClass		*depthBuf;
    Point2D			coord;
    Point3D			point;
    Vector3D			normal;
    float			depth;
  } private;
};

extern Class*			SSAORenderPass;

void		*SSAORenderPassUnserialize();

#endif /* !__SSAORENDERPASS_H__ */

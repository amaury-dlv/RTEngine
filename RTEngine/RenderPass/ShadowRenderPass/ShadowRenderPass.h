#ifndef __SHADOWRENDERPASS_H__
# define __SHADOWRENDERPASS_H__

# include "OOC.h"
# include "RenderPass.h"
# include "LightRenderPass.h"

# define	SHADOWRPASS(x)	((ShadowRenderPassClass *)(x))

typedef struct ShadowRenderPassClass	ShadowRenderPassClass;
struct                          	ShadowRenderPassClass
{
  /* SuperClass : */
  RenderPassClass		__base__;
  /* public : */
  /* private : */
  struct
  {
    LightClass			*curLight;
    LightBufferClass		*curLightBuf;
    PointBufferClass		*pointBuf;
    int				passCount;
    bool			soft;
  } private;
};

extern Class*			ShadowRenderPass;

void		*shadowRenderPassUnserialize();

extern double	g_last_d;
extern double	g_last_t;

#endif /* !__SHADOWRENDERPASS_H__ */

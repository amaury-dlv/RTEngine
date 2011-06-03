#ifndef __LIGHTRENDERPASS_H__
# define __LIGHTRENDERPASS_H__

# include "OOC.h"
# include "RenderPass.h"
# include "LightBuffer.h"
# include "MaterialBuffer.h"
# include "Light.h"

# define	LIGHTRPASS(x)	((LightRenderPassClass *)(x))

typedef struct LightRenderPassClass	LightRenderPassClass;
struct                          	LightRenderPassClass
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
    int				passCount;
  } private;
};

extern Class*			LightRenderPass;

void	*lightRenderPassUnserialize();

#endif

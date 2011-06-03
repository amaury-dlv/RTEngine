#ifndef __SHADERGBRENDERPASS_H__
# define __SHADERGBRENDERPASS_H__

# include "OOC.h"
# include "RenderPass.h"
# include "ColorBuffer.h"
# include "MaterialBuffer.h"

typedef struct ShadeRGBRenderPassClass	ShadeRGBRenderPassClass;
struct                          		ShadeRGBRenderPassClass
{
  /* SuperClass : */
  RenderPassClass		__base__;
  /* public : */
  /* private : */
  struct
  {
    MaterialBufferClass		*mMatBuffer;
    ColorBufferClass		*mBuffer;
    ColorBufferClass		*mCurrentState;
  } private;
};

extern Class*			ShadeRGBRenderPass;

void		*shadeRGBRenderPassUnserialize();

#endif /* !__SHADERGBRENDERPASS_H__ */

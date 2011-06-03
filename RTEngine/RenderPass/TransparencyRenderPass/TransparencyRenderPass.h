#ifndef __TRANSPARENCYRENDERPASS_H__
# define __TRANSPARENCYRENDERPASS_H__

# include "OOC.h"
# include "RenderPass.h"
# include "ColorBuffer.h"
# include "MaterialBuffer.h"

# define TRANSPRPASS(x)	((TransparencyRenderPassClass*) (x))

# define SET_TRANS(x)	(PRIV(mIsTransparent)[(i) / 8] |= (1 << ((i) % 8)))
# define IS_TRANS(x)	(PRIV(mIsTransparent)[(i) / 8] & (1 << ((i) % 8)))

typedef struct TransparencyRenderPassClass	TransparencyRenderPassClass;
struct                          		TransparencyRenderPassClass
{
  /* SuperClass : */
  RenderPassClass		__base__;
  /* public : */
  /* private : */
  struct
  {
    MaterialBufferClass		*mMatBuffer;
    ColorBufferClass		*mTransBuffer;
    int				*mIsTransparent;
    ColorBufferClass		*mCurrentState;
  } private;
};

extern Class*			TransparencyRenderPass;

void		*transparencyRenderPassUnserialize();

#endif /* !__TRANSPARENCYRENDERPASS_H__ */

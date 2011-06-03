#ifndef __TEXTURERENDERPASS_H__
# define __TEXTURERENDERPASS_H__

# include "OOC.h"
# include "RenderPass.h"
# include "ColorBuffer.h"
# include "MaterialBuffer.h"

# define TEXTURERPASS(x)	((TextureRenderPassClass*) (x))

# define SET_TEX(x)	(PRIV(mIsTexture)[(i) / 8] |= (1 << ((i) % 8)))
# define IS_TEX(x)	(PRIV(mIsTexture)[(i) / 8] & (1 << ((i) % 8)))

typedef struct TextureRenderPassClass	TextureRenderPassClass;
struct                          	TextureRenderPassClass
{
  /* SuperClass : */
  RenderPassClass		__base__;
  /* public : */
  /* private : */
  struct
  {
    MaterialBufferClass		*mMatBuffer;
    ColorBufferClass		*mTextureBuffer;
    int				*mIsTexture;
    ColorBufferClass		*mCurrentState;
  } private;
};

extern Class*			TextureRenderPass;

void		*textureRenderPassUnserialize();

#endif /* !__TEXTURERENDERPASS_H__ */

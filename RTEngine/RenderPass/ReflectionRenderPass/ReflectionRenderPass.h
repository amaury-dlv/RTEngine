#ifndef __REFLECTIONRENDERPASS_H__
# define __REFLECTIONRENDERPASS_H__

# include "OOC.h"
# include "RenderPass.h"
# include "ColorBuffer.h"
# include "MaterialBuffer.h"

# define REFLECTRPASS(x)	((ReflectionRenderPassClass*) (x))

# define SET_REFL(x)	(PRIV(mIsReflective)[(i) / 8] |= (1 << ((i) % 8)))
# define IS_REFL(x)	(PRIV(mIsReflective)[(i) / 8] & (1 << ((i) % 8)))

typedef struct ReflectionRenderPassClass	ReflectionRenderPassClass;
struct                          		ReflectionRenderPassClass
{
  /* SuperClass : */
  RenderPassClass		__base__;
  /* public : */
  /* private : */
  struct
  {
    MaterialBufferClass		*mMatBuffer;
    ColorBufferClass		*mReflectBuffer;
    int				*mIsReflective;
    ColorBufferClass		*mCurrentState;
  } private;
};

extern Class*			ReflectionRenderPass;

void		*reflectionRenderPassUnserialize();

#endif /* !__REFLECTIONRENDERPASS_H__ */

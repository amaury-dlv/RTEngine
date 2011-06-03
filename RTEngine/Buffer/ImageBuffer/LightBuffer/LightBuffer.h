#ifndef __LIGHTBUFFER_H__
# define __LIGHTBUFFER_H__

# include "ImageBuffer.h"
# include "Vector3D.h"

# define	LIGHTBUFFER(x)		((LightBufferClass *)(x))

typedef struct 	LightBufferClass	LightBufferClass;
struct                          	LightBufferClass
{
  /* SuperClass : */
  ImageBufferClass		__base__;
  /* public : */
  float				(*getLight)(LightBufferClass* self, int i);
  void				(*offsetSet)(LightBufferClass* self,
                                             int i, float light);
  void				(*set)(LightBufferClass* self,
                                       int x, int y, float light);
  void				(*blur)(LightBufferClass *self, int kernelSize);
  /* private : */
  struct
  {
    float			*mBuffer;
  } private;
};

extern Class*			LightBuffer;

extern float			g_light_blur_weights[13];

#endif /* !__LIGHTBUFFER_H__ */

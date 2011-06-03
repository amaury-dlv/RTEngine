#ifndef __RAYBUFFER_H__
# define __RAYBUFFER_H__

# include "ImageBuffer.h"
# include "Ray.h"

# define RAYBUF(x)	((RayBufferClass*) (x))

typedef struct RayBufferClass		RayBufferClass;
struct                          	RayBufferClass
{
  /* SuperClass : */
  ImageBufferClass		__base__;
  /* public : */
  RayStruct			(*getRay)(RayBufferClass *self, int i);
  RayStruct*			(*getRayPtr)(RayBufferClass *self, int i);
  void				(*setFromStruct)(RayBufferClass *self,
                                                 int x, int y,
                                                 RayStruct *normal);
  /* private : */
  struct
  {
    RayStruct*			mBuffer;
  } private;
};

extern Class*			RayBuffer;

#endif /* !__RAYBUFFER_H__ */

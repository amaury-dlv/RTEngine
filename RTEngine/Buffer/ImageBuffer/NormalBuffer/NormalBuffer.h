#ifndef __NORMALBUFFER_H__
# define __NORMALBUFFER_H__

# include "ImageBuffer.h"
# include "Vector3D.h"

# define	NORMALBUFFER(x)	((NormalBufferClass *)x)

typedef struct NormalBufferClass	NormalBufferClass;
struct                          	NormalBufferClass
{
  /* SuperClass : */
  ImageBufferClass		__base__;
  /* public : */
  Vector3D		(*getNormal)(NormalBufferClass* self, int i);
  Vector3D*		(*getNormalPtr)(NormalBufferClass* self, int i);
  void			(*set)(NormalBufferClass* self,
                               int x, int y, Vector3D normal);
  /* private : */
  struct
  {
    Vector3D*		mBuffer;
  } private;
};

extern Class*			NormalBuffer;

#endif /* !__NORMALBUFFER_H__ */

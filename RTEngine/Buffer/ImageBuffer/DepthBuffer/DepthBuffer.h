#ifndef __DEPTHBUFFER_H__
# define __DEPTHBUFFER_H__

# include "ImageBuffer.h"
# include "HitRecord.h"

# define	DEPTHBUFFER(x)	((DepthBufferClass *)x)

typedef struct DepthBufferClass	DepthBufferClass;
struct                          DepthBufferClass
{
  /* SuperClass : */
  ImageBufferClass		__base__;
  /* public : */
  double			(*getDepth)(DepthBufferClass* self, int i);
  void				(*set)(DepthBufferClass* self,
                                       int x, int y, double depth);
  void				(*setFromHitRecord)(DepthBufferClass* self, int x, int y,
                                                    HitRecordStruct* rec);
  /* private : */
  struct
  {
    double*			mBuffer;
    double			mMaxDist;
  } private;
};

extern Class*			DepthBuffer;

#endif /* !__DEPTHBUFFER_H__ */

#ifndef __POINTBUFFER_H__
# define __POINTBUFFER_H__

# include "ImageBuffer.h"
# include "Point3D.h"

# define	POINTBUFFER(x)		((PointBufferClass *)(x))

typedef struct PointBufferClass		PointBufferClass;
struct                          	PointBufferClass
{
  /* SuperClass : */
  ImageBufferClass		__base__;
  /* public : */
  Point3D		(*getPoint)(PointBufferClass* self, int i);
  Point3D*		(*getPointPtr)(PointBufferClass* self, int i);
  void			(*set)(PointBufferClass* self,
                               int x, int y, Point3D point);
  /* private : */
  struct
  {
    Point3D*		mBuffer;
  } private;
};

extern Class*			PointBuffer;

#endif /* !__POINTBUFFER_H__ */

#ifndef __COLORBUFFER_H__
# define __COLORBUFFER_H__

# include "OOC.h"
# include "ImageBuffer.h"
# include "Color.h"

# define COLORBUF(x)		((ColorBufferClass*) (x))

typedef struct ColorBufferClass	ColorBufferClass;
struct                          ColorBufferClass
{
  /* SuperClass : */
  ImageBufferClass		__base__;
  /* public : */
  Color				(*getColor)(ColorBufferClass *self, int i);
  Color				*(*getColorPtr)(ColorBufferClass *self, int i);
  void				(*offsetSet)(ColorBufferClass *self,
                                             int i, Color color);
  void				(*set)(ColorBufferClass* self,
                                       int x, int y, Color color);
  void				(*reset)(ColorBufferClass *self);
  void				(*setColumnFromBuf)(ColorBufferClass *self,
                                                    Color *buf, int col);
  /* private : */
  struct
  {
    Color*			mBuffer;
  } private;
};

extern Class*			ColorBuffer;

#endif /* !__COLORBUFFER_H__ */

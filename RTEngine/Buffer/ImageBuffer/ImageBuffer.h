#ifndef __IMAGEBUFFER_H__
# define __IMAGEBUFFER_H__

# include "OOC.h"

# define IMAGEBUF(x)		((ImageBufferClass*) (x))

typedef struct ImageBufferClass	ImageBufferClass;
struct                          ImageBufferClass
{
  /* SuperClass : */
  Class				__base__;
  /* public : */
  /* args : self, ColorBufferClass* */
  void				(*convertToColorBuffer)();
  int				id;
  int				width;
  int				height;
  /* private : */
  struct
  {
  } protected;
};

extern Class*			ImageBuffer;

#endif /* !__IMAGEBUFFER_H__ */

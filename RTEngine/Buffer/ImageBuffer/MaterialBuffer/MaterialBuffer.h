#ifndef __MATERIALBUFFER_H__
# define __MATERIALBUFFER_H__

# include "ImageBuffer.h"
# include "Material.h"
# include "Vector3D.h"

# define	MATERIALBUF(x)	((MaterialBufferClass *)(x))

typedef struct MaterialBufferClass	MaterialBufferClass;
struct                          	MaterialBufferClass
{
  /* SuperClass : */
  ImageBufferClass		__base__;
  /* public : */
  MaterialClass			*(*getMaterial)(MaterialBufferClass* self, int i);
  void				(*set)(MaterialBufferClass* self,
                                      int x, int y, MaterialClass *material);
  /* private : */
  struct
  {
    MaterialClass		**mBuffer;
  } private;
};

extern Class*			MaterialBuffer;

#endif /* !__MATERIALBUFFER_H__ */

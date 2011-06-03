#ifndef __GBUFFER_H__
# define __GBUFFER_H__

# include "OOC.h"
# include "Vector.h"
# include "ImageBuffer.h"
# include "LightBuffer.h"

typedef struct GBufferClass	GBufferClass;
struct                          GBufferClass
{
  /* SuperClass : */
  Class				__base__;
  /* public : */
  void				(*addBuffer)(GBufferClass *self,
    ImageBufferClass *buffer);
  ImageBufferClass*		(*getBuffer)(GBufferClass *self, int RTTI);
  LightBufferClass*		(*getLightBuffer)(GBufferClass *self, int id);
  /* private : */
  struct
  {
    VectorClass*		mBuffers;
  } private;
};

extern Class*			GBuffer;

#endif /* !__GBUFFER_H__ */

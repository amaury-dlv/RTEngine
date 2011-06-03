#ifndef __MATERIAL_H__
# define __MATERIAL_H__

# include "OOC.h"
# include "Color.h"
# include "Light.h"
# include "NormalBuffer.h"
# include "PointBuffer.h"
# include "RayBuffer.h"
# include "GBuffer.h"
# include "RTTI.h"
# include "Color.h"

# define MATERIAL(x)		((MaterialClass*) (x))

typedef struct MaterialClass	MaterialClass;
struct                          MaterialClass
{
  /* SuperClass : */
  Class				__base__;
  /* public : */
  /* parameters cannot be defined here because it need HitRecord which need
   * Material.
   * It would have been :
   * MaterialClass *self, HitRecordStruct *hitRecord, LightClass *currentLight
   */
  float				(*shade)(); /* public : */
  Color				(*shadeRGB)();
  Color				(*shadeAreaLight)();
  Color				(*shadePath)();
  Color				color;
  /* protected : */
  struct
  {
    NormalBufferClass		*normalBuffer;
    PointBufferClass		*pointBuffer;
    RayBufferClass		*rayBuffer;
  } protected;
};

extern Class*			Material;

int		materialUnserializeSet();

#endif /* !__MATERIAL_H__ */

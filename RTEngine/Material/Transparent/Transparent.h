#ifndef __TRANSPARENT_H__
# define __TRANSPARENT_H__

# include "Phong.h"
# include "PerfectSpecular.h"
# include "PerfectTransmitter.h"
# include "SceneSerializer.h"
# include "PathTracer.h"
# include "Whitted.h"

# define TRANSPARENT(x)		((TransparentClass*) (x))

typedef struct TransparentHit	TransparentHit;
struct				TransparentHit
{
  RayStruct			reflray;
  RayStruct			transray;
  WhittedClass			*tracer;
  PathTracerClass		*ptracer;
  Color				color;
  Color				fr;
  Color				ft;
  Color				tmp;
  Color				areaColor;
  float				pdf;
  Vector3D			wi;
  Vector3D			wo;
  Vector3D			wt;
};

typedef struct TransparentClass	TransparentClass;
struct                          TransparentClass
{
  /* SuperClass : */
  PhongClass			__base__;
  /* public : */
  /* public : */
  void				(*setKr)(TransparentClass *self, float kr);
  void				(*setCr)(TransparentClass *self, float cr);
  void				(*setKt)(TransparentClass *self, float kt);
  void				(*setIor)(TransparentClass *self, float ior);
  /* private : */
  struct
  {
    PerfectSpecularClass	*mReflectiveBrdf;
    PerfectTransmitterClass	*mSpecularBtdf;
  } private;
};

extern Class*			Transparent;

void	*transparentUnserialize(SceneSerializerClass *serializer,
                           xmlNodePtr node);

MaterialIdAssoc	*transparentUnserializeSet();

#endif /* !__TRANSPARENT_H__ */

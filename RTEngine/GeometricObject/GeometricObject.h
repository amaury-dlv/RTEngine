#ifndef		__GEOMETRICOBJECT_H__
# define	__GEOMETRICOBJECT_H__

# include "OOC.h"
# include "Material.h"
# include "Matte.h"
# include "Matrix.h"
# include "Transformation.h"
# include "Sampler.h"

# define GEOOBJECT(x)		((GeometricObjectClass*) (x))

typedef struct GeometricObjectClass	GeometricObjectClass;
struct                          	GeometricObjectClass
{
  /* SuperClass : */
  Class				__base__;
  /* public : */
  int				(*getId)(GeometricObjectClass* self);
  bool				(*objHit)();
  bool				(*objHitWithRecord)();
  bool				(*hit)();
  bool				(*hitWithRecord)();
  Vector3D			(*getNormal)();
  float				(*pdf)();
  Vector3D			(*sample)();
  int				mId;
  MaterialClass			*material;
  Point3D			mPos;
  struct
  {
    t_matrix			*mMatrix;
    float			mInvArea;
    SamplerClass		*mSampler;
  } private;
};

/*
** Never new it !!!
*/
extern Class*			GeometricObject;

int		geometricObjectUnserializeSet();

#endif		/* ! __GEOMETRICOBJECT_H__ */

#ifndef __FLATMESHTRIANGLE_H__
# define __FLATMESHTRIANGLE_H__

# include "MeshTriangle.h"
# include "Vector3D.h"

typedef struct FlatMeshTriangleClass	FlatMeshTriangleClass;
struct					FlatMeshTriangleClass
{
  MeshTriangleClass		__base__;
  /* public */
  /* private */
  struct
  {
  } private;
};

extern Class*			FlatMeshTriangle;

#endif /* !__FLATMESHTRIANGLE_H__ */

#ifndef __SMOOTHMESHTRIANGLE_H__
# define __SMOOTHMESHTRIANGLE_H__

# include "MeshTriangle.h"
# include "Vector3D.h"

# define SMOOTH(x)	((SmoothMeshTriangleClass*) (x))

typedef struct SmoothMeshTriangleClass	SmoothMeshTriangleClass;
struct					SmoothMeshTriangleClass
{
  MeshTriangleClass		__base__;
  /* public */
  /* private */
};

extern Class*			SmoothMeshTriangle;

#endif /* !__SMOOTHMESHTRIANGLE_H__ */

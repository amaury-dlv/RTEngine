#ifndef __MESHTRIANGLE_H__
# define __MESHTRIANGLE_H__

# include <libxml/xmlmemory.h>
# include <libxml/parser.h>

# include "GeometricObject.h"
# include "Mesh.h"
# include "Vector3D.h"
# include "Point3D.h"
# include "SceneSerializer.h"
# include "BBox.h"

# define TRIANGLE(x)		((MeshTriangleClass*) (x))

typedef struct TriangleHit		TriangleHit;
struct					TriangleHit
{
  Point3D				*v0;
  Point3D				*v1;
  Point3D				*v2;
  double				a;
  double				b;
  double				c;
  double				d;
  double				e;
  double				f;
  double				g;
  double				h;
  double				i;
  double				j;
  double				k;
  double				l;
  double				m;
  double				n;
  double				p;
  double				q;
  double				s;
  double				inv_denom;
  double				e1;
  double				beta;
  double				r;
  double				e2;
  double				gamma;
  double				e3;
  double				t;
  int					ret;
};

typedef struct MeshTriangleClass	MeshTriangleClass;
struct                          	MeshTriangleClass
{
  /* SuperClass : */
  GeometricObjectClass		__base__;
  /* public : */
  void				(*computeNormal)(MeshTriangleClass *self, bool reverse);
  /* private : */
  MeshClass			*mesh;
  int				index0;
  int				index1;
  int				index2;
  Vector3D			normal;
};

extern Class*			MeshTriangle;
extern double			triangleKEpsilon;

BBox	meshTriangleGetBBox(MeshTriangleClass *self);

void	meshTriangleHitInit(MeshTriangleClass *self,
                            RayStruct *ray,
                            double *tMin,
                            TriangleHit *h);

#endif /* !__MESHTRIANGLE_H__ */

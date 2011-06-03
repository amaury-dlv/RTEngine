#include "MeshTriangle.h"
#include "HitRecord.h"
#include "Maths.h"

static void     meshTriangleConstructor();
static void     meshTriangleDestructor(MeshTriangleClass*self);
static bool	meshTriangleHit(MeshTriangleClass *self,
                                RayStruct *ray, double *tMin);
static void	meshTriangleComputeNormals(MeshTriangleClass *self,
                                           bool reverse);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static MeshTriangleClass       _descr = { /* MeshTriangleClass */
  { /* GeometricObjectClass */
    { /* Class */
      sizeof(MeshTriangleClass),
      __RTTI_TRIANGLE, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
      &meshTriangleConstructor,
      &meshTriangleDestructor,
    },
    0, /* GeometricObjectNE1.getId will be set in the constructor */
    0, /* geometricObjectHit */
    0, /* geometricObjectHitWithRecord */
    0, /* hit() = 0; Pure virtual */
    0, /* hitWithRecord() = 0; Pure virtual */
    0, /* getNormal() ; virtual */
    0, /* pdf() ; virtual */
    0, /* sample() */
    0, /* mId */
    0, /* material */
    {0, 0, 0}, /* mPos */
    {
      0, /* mRot */
      0, /* mInvArea */
      0 /* mSampler */
    }
  },
  0, /* computeNormal */
  0, /* mMesh */
  0, /* mIndex0 */
  0, /* mIndex1 */
  0, /* mIndex2 */
  {0, 0, 0}, /* mNormal */
};

Class *MeshTriangle = (Class*) &_descr;

double triangleKEpsilon = 0.00001;

static void     meshTriangleConstructor(MeshTriangleClass *self, va_list *list)
{
  GeometricObject->__constructor__(self, list);
  GEOOBJECT(self)->objHit = &meshTriangleHit;
  self->computeNormal = &meshTriangleComputeNormals;
  self->mesh = va_arg(*list, MeshClass*);
  self->index0 = va_arg(*list, int);
  self->index1 = va_arg(*list, int);
  self->index2 = va_arg(*list, int);
}

static void     meshTriangleDestructor(MeshTriangleClass *self)
{
  GeometricObject->__destructor__(self);
}

void		meshTriangleHitInit(MeshTriangleClass *self,
                                    RayStruct *ray,
                                    double *tMin,
                                    TriangleHit *h)
{
  h->v0 = self->mesh->vertices->mPtrs[self->index0];
  h->v1 = self->mesh->vertices->mPtrs[self->index1];
  h->v2 = self->mesh->vertices->mPtrs[self->index2];
  h->a = h->v0->x - h->v1->x, h->b = h->v0->x - h->v2->x;
  h->c = ray->direction.x, h->d = h->v0->x - ray->origin.x;
  h->e = h->v0->y - h->v1->y, h->f = h->v0->y - h->v2->y;
  h->g = ray->direction.y, h->h = h->v0->y - ray->origin.y;
  h->i = h->v0->z - h->v1->z, h->j = h->v0->z - h->v2->z;
  h->k = ray->direction.z, h->l = h->v0->z - ray->origin.z;
  h->m = h->f * h->k - h->g * h->j, h->n = h->h * h->k - h->g * h->l;
  h->p = h->f * h->l - h->h * h->j;
  h->q = h->g * h->i - h->e * h->k, h->s = h->e * h->j - h->f * h->i;
  h->inv_denom  = 1.0 / (h->a * h->m + h->b * h->q + h->c * h->s);
  h->e1 = h->d * h->m - h->b * h->n - h->c * h->p;
  h->beta = h->e1 * h->inv_denom;
}


BBox	meshTriangleGetBBox(MeshTriangleClass *self)
{
  Point3D	*v1 = (Point3D*)self->mesh->vertices->mPtrs[self->index0];
  Point3D	*v2 = (Point3D*)self->mesh->vertices->mPtrs[self->index1];
  Point3D	*v3 = (Point3D*)self->mesh->vertices->mPtrs[self->index2];

#define delta 0.000001
  return ((BBox) {
      MIN(MIN(v1->x, v2->x), v3->x) - delta,
      MAX(MAX(v1->x, v2->x), v3->x) + delta,
      MIN(MIN(v1->y, v2->y), v3->y) - delta,
      MAX(MAX(v1->y, v2->y), v3->y) + delta,
      MIN(MIN(v1->z, v2->z), v3->z) - delta,
      MAX(MAX(v1->z, v2->z), v3->z) + delta
      });
#undef delta
}

static bool	meshTriangleHit(MeshTriangleClass *self,
                                RayStruct *ray,
                                double *tMin)
{
  TriangleHit	h;

  meshTriangleHitInit(self, ray, tMin, &h);
  if (h.beta < 0.)
    return (false);
  h.r = h.r = h.e * h.l - h.h * h.i;
  h.e2 = h.a * h.n + h.d * h.q + h.c * h.r;
  h.gamma = h.e2 * h.inv_denom;
  if (h.gamma < 0.0 )
            return (false);
  if (h.beta + h.gamma > 1.0)
            return (false);
  h.e3 = h.a * h.p - h.b * h.r + h.d * h.s;
  h.t = h.e3 * h.inv_denom;
  if (h.t < triangleKEpsilon)
            return (false);
  *tMin = h.t;
  return (true);
}

/*
 * normal = (mesh_ptr->vertices[index1] - mesh_ptr->vertices[index0]) ^
 *   (mesh_ptr->vertices[index2] - mesh_ptr->vertices[index0]);
 */
static void	meshTriangleComputeNormals(MeshTriangleClass *self,
                                           bool reverse)
{
  Vector3D	a;
  Vector3D	b;

  a = *(Point3D*)self->mesh->vertices->mPtrs[self->index1];
  vectOpMinus(&(a), self->mesh->vertices->mPtrs[self->index0]);
  b = *(Point3D*)self->mesh->vertices->mPtrs[self->index2];
  vectOpMinus(&(b), self->mesh->vertices->mPtrs[self->index0]);
  vectCrossProduct(&(a), &(b));
  self->normal = a;
  vectNormalize(&(self->normal));
  if (reverse)
    vectInv(&(self->normal));
}

#include "FlatMeshTriangle.h"
#include "HitRecord.h"

static void     flatMeshTriangleConstructor();
static void     flatMeshTriangleDestructor(FlatMeshTriangleClass *self);
static bool	flatMeshTriangleHitWithRecord(MeshTriangleClass *self,
                                              RayStruct *ray,
                                              double *tMin,
                                              HitRecordStruct *hitRecord);

static FlatMeshTriangleClass	_descr = { /* FlatMeshTriangle */
  { /* MesTriangleClass */
    { /* GeometricObjectClass */
      { /* Class */
        sizeof(FlatMeshTriangleClass),
        __RTTI_TRIANGLE,
        &flatMeshTriangleConstructor,
        &flatMeshTriangleDestructor,
      },
      0, /* GeometricObjectNE1.getId will be set in the constructor */
      0,
      &flatMeshTriangleHitWithRecord,
      0, /* geometricObjectHit */
      0, /* geometricObjectHitWithRecord */
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
  },
  {
  }
};

Class *FlatMeshTriangle = (Class*) &_descr;

static void     flatMeshTriangleConstructor(FlatMeshTriangleClass *self,
                                            va_list *list)
{
  MeshTriangle->__constructor__(self, list);
}

static void     flatMeshTriangleDestructor(FlatMeshTriangleClass *self)
{
  MeshTriangle->__destructor__(self);
}

static bool	flatMeshTriangleHitWithRecord(MeshTriangleClass *self,
                                              RayStruct *ray,
                                              double *tMin,
                                              HitRecordStruct *hitRecord)
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
  hitRecord->normal = self->normal;
  vectInv(&hitRecord->normal);
  hitRecord->hitPoint = hitRecord->ray.direction;
  vectOpTimes(&(hitRecord->hitPoint), h.t);
  vectOpPlus(&(hitRecord->hitPoint), &(hitRecord->ray.origin));
  hitRecord->material = GEOOBJECT(self)->material;
  return (true);
}

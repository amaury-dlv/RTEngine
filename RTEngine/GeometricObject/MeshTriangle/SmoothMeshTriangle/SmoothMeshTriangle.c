#include "SmoothMeshTriangle.h"
#include "HitRecord.h"

static void     smoothMeshTriangleConstructor();
static void     smoothMeshTriangleDestructor(SmoothMeshTriangleClass *self);
static void	smoothMeshTriangleInterpolateNormal(
    				SmoothMeshTriangleClass *self,
                                                    float beta,
                                                    float gamma,
                                                    Vector3D *res);
static bool	smoothMeshTriangleHitWithRecord(MeshTriangleClass *self,
                                              RayStruct *ray,
                                              double *tMin,
                                              HitRecordStruct *hitRecord);

static SmoothMeshTriangleClass	_descr = { /* SmoothMeshTriangle */
  { /* MesTriangleClass */
    { /* GeometricObjectClass */
      { /* Class */
        sizeof(SmoothMeshTriangleClass),
        __RTTI_TRIANGLE,
        &smoothMeshTriangleConstructor,
        &smoothMeshTriangleDestructor,
      },
      0, /* GeometricObjectNE1.getId will be set in the constructor */
      0,
      &smoothMeshTriangleHitWithRecord,
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
  },
};

Class *SmoothMeshTriangle = (Class*) &_descr;

static void     smoothMeshTriangleConstructor(SmoothMeshTriangleClass *self,
                                              va_list *list)
{
  MeshTriangle->__constructor__(self, list);
}

static void     smoothMeshTriangleDestructor(SmoothMeshTriangleClass *self)
{
  MeshTriangle->__destructor__(self);
}

/*
 * normal((1 - beta - gamma) * n0 + beta * n1 + gamma * n2)
 */
static void	smoothMeshTriangleInterpolateNormal(
                                        SmoothMeshTriangleClass *self,
                                        float beta,
                                        float gamma,
                                        Vector3D *res)
{
  Vector3D	a;
  Vector3D	b;
  Vector3D	c;

  a = *(Point3D*)TRIANGLE(self)->mesh->normals->mPtrs[TRIANGLE(self)->index0];
  vectOpTimes(&(a), 1.f - beta - gamma);
  b = *(Point3D*)TRIANGLE(self)->mesh->normals->mPtrs[TRIANGLE(self)->index1];
  vectOpTimes(&(b), beta);
  c = *(Point3D*)TRIANGLE(self)->mesh->normals->mPtrs[TRIANGLE(self)->index2];
  vectOpTimes(&(c), gamma);
  *res = a;
  vectOpPlus(res, &(b));
  vectOpPlus(res, &(c));
  vectNormalize(res);
}

static bool	smoothMeshTriangleHitWithRecord(MeshTriangleClass *self,
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
  smoothMeshTriangleInterpolateNormal(SMOOTH(self),
      h.beta, h.gamma, &(hitRecord->normal));
  vectInv(&hitRecord->normal);
  hitRecord->hitPoint = hitRecord->ray.direction;
  vectOpTimes(&(hitRecord->hitPoint), h.t);
  vectOpPlus(&(hitRecord->hitPoint), &(hitRecord->ray.origin));
  hitRecord->material = GEOOBJECT(self)->material;
  return (true);
}

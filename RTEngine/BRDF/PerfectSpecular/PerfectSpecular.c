#include "PerfectSpecular.h"

static void     PerfectSpecularConstructor();
static void     PerfectSpecularDestructor(PerfectSpecularClass *self);
static float	PerfectSpecularF(PerfectSpecularClass *self,
                                HitRecordStruct *hitRecord,
                                Vector3D *wo, Vector3D *wi);
static Color	PerfectSpecularSampleF(PerfectSpecularClass *self,
                                      HitRecordStruct *hitRecord,
                                      Vector3D *wo, Vector3D *wi);
static Color	PerfectSpecularSampleFPath(PerfectSpecularClass *self,
                                      HitRecordStruct *hitRecord,
                                      Vector3D *wo, Vector3D *wi,
                                      float *pdf);
static float	PerfectSpecularRho(PerfectSpecularClass *self,
                                  HitRecordStruct *hitRecord,
                                  Vector3D *wo, Vector3D *wi);
static void	PerfectSpecularSetKr(PerfectSpecularClass *self, float kr);
static void	PerfectSpecularSetCr(PerfectSpecularClass *self, float cr);

static PerfectSpecularClass       _descr = { /* PerfectSpecularClass */
  { /* BRDFClass */
    { /* Class */
      sizeof(PerfectSpecularClass),
      0, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
      &PerfectSpecularConstructor,
      &PerfectSpecularDestructor,
    },
    &PerfectSpecularF,
    &PerfectSpecularSampleF,
    &PerfectSpecularSampleFPath,
    &PerfectSpecularRho,
    0,
    {
      0 /* mSamplerType */
    }
  },
  &PerfectSpecularSetKr,
  &PerfectSpecularSetCr,
  { /* private */
    1., /* mKr */
    1. /* mCr */
  }
};

Class *PerfectSpecular = (Class*) &_descr;

static void     PerfectSpecularConstructor(PerfectSpecularClass *self,
                                           va_list *list)
{
  BRDF->__constructor__((BRDFClass *) self, list);
}

static void     PerfectSpecularDestructor(PerfectSpecularClass *self)
{
}

static float	PerfectSpecularF(PerfectSpecularClass *self,
                                HitRecordStruct *hitRecord,
                                Vector3D *wo, Vector3D *wi)
{
  return (0.);
}

static Color	PerfectSpecularSampleF(PerfectSpecularClass *self,
                                      HitRecordStruct *hitRecord,
                                      Vector3D *wo, Vector3D *wi)
{
  float		nDotWo;
  float		k;

  nDotWo = vectDotProduct(&hitRecord->normal, wo);
  *wi = hitRecord->normal;
  vectOpTimes(wi, nDotWo);
  vectOpTimes(wi, 2.0f);
  vectOpTimes(wo, -1.f);
  vectOpPlus(wi, wo);
  vectOpTimes(wo, -1.f);
  k = (self->private.mKr * self->private.mCr)
    / vectDotProduct(&hitRecord->normal, wi);
  return ((Color) {k, k, k});
}

static Color	PerfectSpecularSampleFPath(PerfectSpecularClass *self,
                                      HitRecordStruct *hitRecord,
                                      Vector3D *wo, Vector3D *wi,
                                      float *pdf)
{
  float		nDotWo = vectDotProduct(&hitRecord->normal, wo);
  *wi = hitRecord->normal;
  vectOpTimes(wi, 2. * nDotWo);
  vectInv(wo);
  vectOpPlus(wi, wo);
  vectInv(wo);
  *pdf = vectDotProduct(&hitRecord->normal, wi);
  float		k = self->private.mKr * self->private.mCr;
  return ((Color){k, k, k});
}

static float	PerfectSpecularRho(PerfectSpecularClass *self,
                                  HitRecordStruct *hitRecord,
                                  Vector3D *wo, Vector3D *wi)
{
  return (0.);
}

static void	PerfectSpecularSetKr(PerfectSpecularClass *self, float kr)
{
  self->private.mKr = kr;
}

static void	PerfectSpecularSetCr(PerfectSpecularClass *self, float cr)
{
  self->private.mCr = cr;
}

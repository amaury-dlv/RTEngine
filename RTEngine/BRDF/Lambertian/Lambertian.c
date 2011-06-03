#include "Lambertian.h"
#include "RTEngine.h"

static void     lambertianConstructor(LambertianClass *self, va_list *list);
static void     lambertianDestructor(LambertianClass *self);
static void	lambertianSetKd(LambertianClass *self, float kd);
static float	lambertianF(LambertianClass *self, HitRecordStruct *hitRecord,
                            Vector3D *wo, Vector3D *wi);
static Color	lambertianSampleFPath(LambertianClass *self,
                                      HitRecordStruct *hitRecord,
                                      Vector3D *wo, Vector3D *wi, float *pdf);
static float	lambertianRho(LambertianClass *self,
                              HitRecordStruct *hitRecord,
                              Vector3D *wo, Vector3D *wi);

static LambertianClass       _descr = { /* LambertianClass */
  { /* BRDFClass */
    { /* Class */
      sizeof(LambertianClass),
      0, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
      &lambertianConstructor,
      &lambertianDestructor,
    },
    &lambertianF,
    0, /* samplef */
    &lambertianSampleFPath,
    &lambertianRho,
    0,
    {
      0 /* mSamplerType */
    }
  },
  &lambertianSetKd,
  { /* private */
    0, /* mAttenuation */
    1. /* mKd */
  }
};

Class *Lambertian = (Class*) &_descr;

static void     lambertianConstructor(LambertianClass *self, va_list *list)
{
  BRDF->__constructor__((BRDFClass *) self, list);
  self->private.mAttenuation = RTEngineGetSingletonPtr()\
                               ->getLightAttenuation();
}

static void     lambertianDestructor(LambertianClass *self)
{
}

static float	lambertianF(LambertianClass *self, HitRecordStruct *hitRecord,
                            Vector3D *wo, Vector3D *wi)
{
  if (self->private.mAttenuation)
    return (self->private.mKd * InvPi
            / (hitRecord->distLight * hitRecord->distLight));
  return (self->private.mKd * InvPi);
}

static Color	lambertianSampleFPath(LambertianClass *self,
                                      HitRecordStruct *hitRecord,
                                      Vector3D *wo, Vector3D *wi,
                                      float *pdf)
{
  Vector3D	w;
  Vector3D	v;
  Vector3D	u;
  Point3D	sp;
  float		k;

  w = hitRecord->normal;
  v = (Vector3D){0.0034, 1.0, 0.0071};
  vectCrossProduct(&v, &w);
  vectNormalize(&v);
  u = v;
  vectCrossProduct(&u, &w);
  sp = BRDFT(self)->protected.mSampler\
         ->nextHemisphereSample(BRDFT(self)->protected.mSampler);
  wi->x = sp.x * u.x + sp.y * v.x + sp.z * w.x;
  wi->y = sp.x * u.y + sp.y * v.y + sp.z * w.y;
  wi->z = sp.x * u.z + sp.y * v.z + sp.z * w.z;
  vectNormalize(wi);
  *pdf = vectDotProduct(&hitRecord->normal, wi) * InvPi;
  k = self->private.mKd * InvPi;
  return ((Color) {k, k, k});
}

static float	lambertianRho(LambertianClass *self,
                              HitRecordStruct *hitRecord,
                              Vector3D *wo, Vector3D *wi)
{
  return (self->private.mKd);
}

static void	lambertianSetKd(LambertianClass *self, float kd)
{
  self->private.mKd = kd;
}

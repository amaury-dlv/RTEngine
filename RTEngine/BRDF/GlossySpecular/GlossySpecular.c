#include <math.h>

#include "GlossySpecular.h"
#include "Vector3D.h"

static void     glossySpecularConstructor();
static void     glossySpecularDestructor(GlossySpecularClass *self);
static void	glossySpecularSetKs(GlossySpecularClass *self, float kd);
static float	glossySpecularF(GlossySpecularClass *self,
                                HitRecordStruct *hitRecord,
                                Vector3D *wo, Vector3D *wi);
static Color	glossySpecularSampleF(GlossySpecularClass *self,
                                      HitRecordStruct *hitRecord,
                                      Vector3D *wo, Vector3D *wi);
static Color	glossySpecularSampleFPath(GlossySpecularClass *self,
                                      HitRecordStruct *hitRecord,
                                      Vector3D *wo, Vector3D *wi,
				      float *pdf);
static float	glossySpecularRho(GlossySpecularClass *self,
                                  HitRecordStruct *hitRecord,
                                  Vector3D *wo, Vector3D *wi);
static void	glossySpecularSetKs(GlossySpecularClass *self, float kd);
static void	glossySpecularSetCr(GlossySpecularClass *self, float cr);
static void	glossySpecularSetExp(GlossySpecularClass *self, float exp);
static Vector3D	glossySpecularSampleFOutside(Vector3D *u, Vector3D *v,
                                             Vector3D *w, Vector3D *s);

static GlossySpecularClass       _descr = { /* GlossySpecularClass */
  { /* BRDFClass */
    { /* Class */
      sizeof(GlossySpecularClass),
      0, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
      &glossySpecularConstructor,
      &glossySpecularDestructor,
    },
    &glossySpecularF,
    VOID(&glossySpecularSampleF),
    VOID(&glossySpecularSampleFPath),
    &glossySpecularRho,
    0,
    {
      0 /* mSampler */
    }
  },
  &glossySpecularSetKs,
  &glossySpecularSetCr,
  &glossySpecularSetExp,
  { /* private */
    1., /* mKd */
    1., /* mCr */
    1. /* mExp */
  }
};

Class *GlossySpecular = (Class*) &_descr;

static void     glossySpecularConstructor(GlossySpecularClass *self,
                                          va_list *list)
{
  BRDF->__constructor__((BRDFClass *) self, list);
}

static void     glossySpecularDestructor(GlossySpecularClass *self)
{
}

static float	glossySpecularF(GlossySpecularClass *self,
                                HitRecordStruct *hitRecord,
                                Vector3D *wo, Vector3D *wi)
{
  float			nDotWi;
  Vector3D	r;
  float			rDotWo;
  float			l;

  nDotWi = wi->x * hitRecord->normal.x
    + wi->y * hitRecord->normal.y
    + wi->z * hitRecord->normal.z;
  nDotWi = -nDotWi;
  r =
    (Vector3D) {
      wi->x + hitRecord->normal.x * nDotWi * 2.,
      wi->y + hitRecord->normal.y * nDotWi * 2.,
      wi->z + hitRecord->normal.z * nDotWi * 2.
    };
  rDotWo = wo->x * r.x + wo->y * r.y + wo->z * r.z;
  if (rDotWo <= 0.)
    return (.0);
  l = self->private.mKs * pow(rDotWo, self->private.mExp);
  return (l);
}

static Vector3D	glossySpecularSampleFOutside(Vector3D *u, Vector3D *v,
                                             Vector3D *w, Vector3D *s)
{
  Vector3D	r;
  Vector3D	tmp;

  r = *u;
  vectOpTimes(&r, s->x);
  tmp = *v;
  vectOpTimes(&tmp, s->y);
  vectOpPlus(&r, &tmp);
  tmp = *w;
  vectOpTimes(&tmp, s->z);
  vectOpPlus(&r, &tmp);
  return (r);
}

static Color	glossySpecularSampleF(GlossySpecularClass *self,
                                      HitRecordStruct *hitRecord,
                                      Vector3D *wo, Vector3D *wi)
{
  Vector3D	t[6];
  float		k;

  t[0] = hitRecord->normal;
  vectOpTimes(&t[0], 2. * vectDotProduct(&hitRecord->normal, wo));
  vectOpMinus(&t[0], wo);
  t[4] = t[0];
  t[1] = (Vector3D){0.00424, 1, 0.00764};
  vectCrossProduct(&t[1], &t[4]);
  vectNormalize(&t[1]);
  t[2] = t[1];
  vectCrossProduct(&t[2], &t[4]);
  t[5] = BRDFT(self)->protected.mSampler\
    ->nextHemisphereSample(BRDFT(self)->protected.mSampler);
  *wi = glossySpecularSampleFOutside(&t[1], &t[2], &t[4], &t[5]);
  if (vectDotProduct(&hitRecord->normal, wi) < 0.)
  {
    t[5] = (Vector3D){-t[5].x, -t[5].y, t[5].z};
    *wi = glossySpecularSampleFOutside(&t[1], &t[2], &t[4], &t[5]);
  }
  k = self->private.mKs * self->private.mCr * pow(vectDotProduct(&t[0], wi),
    PRIV(mExp));
  return ((Color){k, k, k});
}

static Color	glossySpecularSampleFPath(GlossySpecularClass *self,
                                      HitRecordStruct *hitRecord,
                                      Vector3D *wo, Vector3D *wi,
				      float *pdf)
{
  Vector3D	t[6];
  float		k;
  float		phong_lobe;

  t[0] = hitRecord->normal;
  vectOpTimes(&t[0], 2. * vectDotProduct(&hitRecord->normal, wo));
  vectOpMinus(&t[0], wo);
  t[4] = t[0];
  t[1] = (Vector3D){0.00424, 1, 0.00764};
  vectCrossProduct(&t[1], &t[4]);
  vectNormalize(&t[1]);
  t[2] = t[1];
  vectCrossProduct(&t[2], &t[4]);
  t[5] = BRDFT(self)->protected.mSampler\
    ->nextHemisphereSample(BRDFT(self)->protected.mSampler);
  *wi = glossySpecularSampleFOutside(&t[1], &t[2], &t[4], &t[5]);
  if (vectDotProduct(&hitRecord->normal, wi) < 0.)
  {
    t[5] = (Vector3D){-t[5].x, -t[5].y, t[5].z};
    *wi = glossySpecularSampleFOutside(&t[1], &t[2], &t[4], &t[5]);
  }
  phong_lobe = pow(vectDotProduct(&t[0], wi), PRIV(mExp));
  *pdf = phong_lobe * vectDotProduct(&hitRecord->normal, wi);
  k = self->private.mKs * self->private.mCr * phong_lobe;
  return ((Color){k, k, k});
}

static float	glossySpecularRho(GlossySpecularClass *self,
                                  HitRecordStruct *hitRecord,
                                  Vector3D *wo, Vector3D *wi)
{
  return (0.);
}

static void	glossySpecularSetCr(GlossySpecularClass *self, float cr)
{
  self->private.mCr = cr;
}

static void	glossySpecularSetKs(GlossySpecularClass *self, float ks)
{
  self->private.mKs = ks;
}

static void	glossySpecularSetExp(GlossySpecularClass *self, float exp)
{
  self->private.mExp = exp;
  BRDFT(self)->protected.mSampler\
    ->mapSamplesToHemisphere(BRDFT(self)->protected.mSampler, PRIV(mExp));
}

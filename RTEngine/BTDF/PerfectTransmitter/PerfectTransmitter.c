#include <math.h>

#include "PerfectTransmitter.h"

static void     PerfectTransmitterConstructor();
static void     PerfectTransmitterDestructor(PerfectTransmitterClass *self);
static void	perfectTransmitterSetKt(PerfectTransmitterClass *self,
                                        float kt);
static void	perfectTransmitterSetIor(PerfectTransmitterClass *self,
                                         float ior);
static bool	perfectTransmitterTir(PerfectTransmitterClass *self,
                                      HitRecordStruct *hitRecord);
static Color	perfectTransmitterSampleF(PerfectTransmitterClass *self,
                                          HitRecordStruct *rec,
                                          Vector3D *wo,
                                          Vector3D *wt);

static PerfectTransmitterClass       _descr = { /* PerfectTransmitter */
  { /* BTDFClass */
    { /* Class */
      sizeof(PerfectTransmitterClass),
      0, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
      &PerfectTransmitterConstructor,
      &PerfectTransmitterDestructor,
    },
    0, /* f() */
    &perfectTransmitterSampleF, /* sampleF() */
    0, /* rho() */
    &perfectTransmitterTir, /* tir */
    { /* protected */
    }
  },
  &perfectTransmitterSetKt,
  &perfectTransmitterSetIor,
  { /* private */
    0., /* mKt */
    0. /* mIot */
  }
};

Class *PerfectTransmitter = (Class*) &_descr;

static void     PerfectTransmitterConstructor(PerfectTransmitterClass *self,
                                              va_list *list)
{
}

static void     PerfectTransmitterDestructor(PerfectTransmitterClass *self)
{
}

static void	perfectTransmitterSetKt(PerfectTransmitterClass *self,
                                        float kt)
{
  self->private.mKt = kt;
}

static void	perfectTransmitterSetIor(PerfectTransmitterClass *self,
                                         float ior)
{
  self->private.mIor = ior;
}

static bool	perfectTransmitterTir(PerfectTransmitterClass *self,
                                      HitRecordStruct *hitRecord)
{
  Vector3D	wo;
  float		cos_thetai;
  float		eta;

  wo = hitRecord->ray.direction;
  vectOpTimes(&wo, -1);
  cos_thetai = vectDotProduct(&hitRecord->normal, &wo);
  eta = self->private.mIor;
  if (cos_thetai < 0)
    eta = 1 / eta;
  return ((1 - (1 - cos_thetai * cos_thetai) / (eta * eta)) < 0);
}

static Color	perfectTransmitterSampleF(PerfectTransmitterClass *self,
                                          HitRecordStruct *rec,
                                          Vector3D *wo,
                                          Vector3D *wt)
{
  Vector3D	n;
  float		cos_theta_i;
  float		eta;
  float		k;
  float		cos_theta2;

  n = rec->normal;
  cos_theta_i = vectDotProduct(&n, wo);
  eta = self->private.mIor;
  if (cos_theta_i < 0.)
  {
    cos_theta_i = -cos_theta_i;
    vectOpTimes(&n, -1.f);
    eta = 1.f / eta;
  }
  cos_theta2 = sqrtf(1.f - (1.f - cos_theta_i * cos_theta_i) / (eta * eta));
  *wt = *wo;
  vectOpTimes(wt, -1.f / eta);
  vectOpTimes(&n, -(cos_theta2 - cos_theta_i / eta));
  vectOpPlus(wt, &n);
  k = self->private.mKt
    / (eta * eta) * 1.f / fabsf(vectDotProduct(&rec->normal, wt));
  return ((Color) {k, k, k});
}

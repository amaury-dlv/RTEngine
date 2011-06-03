#include "Transparent.h"
#include "Lambertian.h"
#include "GlossySpecular.h"
#include "Utils.h"
#include "TracerManager.h"

static void	transparentConstructor(TransparentClass *self, va_list *list);
static void	transparentDestructor(TransparentClass *self);
static void	transparentSetCr(TransparentClass *self, float cr);
static void	transparentSetKr(TransparentClass *self, float kr);
static void	transparentSetKt(TransparentClass *self, float kt);
static void	transparentSetIor(TransparentClass *self, float ior);
static float	transparentShade(TransparentClass *self,
                                 HitRecordStruct *hitRecord,
                                 LightClass *currentLight);
static void	transparentShadeRGBCol(TransparentClass *self,
                                       HitRecordStruct *hitRecord,
                                       TransparentHit *c);
static Color	transparentShadeRGB(TransparentClass *self,
                                    HitRecordStruct *hitRecord);
static Color	transparentShadePath(TransparentClass *self,
                                     HitRecordStruct *hitRecord);

static TransparentClass       _descr = { /* TransparentClass */
  { /* PhongClass */
    { /* MaterialClass */
      { /* Class */
        sizeof(TransparentClass),
        __RTTI_TRANSPARENT, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
        &transparentConstructor,
        &transparentDestructor,
      },
      /* public */
      &transparentShade, /* transparentShade */
      &transparentShadeRGB,
      0, /* shadeAreaLight */
      &transparentShadePath, /* shadePath */
      /* public */
      {0., 0., 0.}, /* color */
      /* protected */
      {
        0, /* normalBuffer */
        0, /* pointBuffer */
        0 /* rayBuffer */
      }
    },
    0, /* setters */
    0, /* setters */
    0, /* setters */
    { /* private */
      0, /* mDiffuseBrdf */
      0 /* mGlossySpecularBrdf */
    }
  },
  &transparentSetKr,
  &transparentSetCr,
  &transparentSetKt,
  &transparentSetIor,
  { /* private */
    0,
    0
  }
};

Class *Transparent = (Class*) &_descr;

static void	transparentConstructor(TransparentClass *self, va_list *list)
{
  Phong->__constructor__(self, list);
  self->private.mReflectiveBrdf = new(PerfectSpecular, eRegular);
  self->private.mSpecularBtdf = new(PerfectTransmitter, eRegular);
  MATERIAL(self)->shadeRGB = &transparentShadeRGB;
  MATERIAL(self)->shadePath = &transparentShadePath;
  self->setKr = &transparentSetKr;
  self->setCr = &transparentSetCr;
  self->setKt = &transparentSetKt;
  self->setIor = &transparentSetIor;
}

static void	transparentDestructor(TransparentClass *self)
{
  delete(self->private.mReflectiveBrdf);
  delete(self->private.mSpecularBtdf);
}

void	*transparentUnserialize(SceneSerializerClass *serializer,
                           xmlNodePtr node)
{
  TransparentClass	*self;
  MaterialIdAssoc	*assoc;
  char			*strKr, *strCr, *strKt, *strIor;
  float			kr, cr, kt, ior;

  self = new(Transparent);
  if ((assoc = phongUnserializeSet(serializer, node, PHONG(self))) == 0)
    return (0);
  serializer->setStrsFromProp(serializer, node, "kr", &strKr, "cr",
      &strCr, "kt", &strKt, "ior", &strIor, NULL);
  if (setFloatFromStr(strKr, &kr) != 0
      || setFloatFromStr(strCr, &cr) != 0
      || setFloatFromStr(strKt, &kt) != 0
      || setFloatFromStr(strIor, &ior) != 0)
  {
    fprintf(stderr, "Error: bad attributes\n");
    return (0);
  }
  self->setKr(self, kr);
  self->setCr(self, cr);
  self->setKt(self, kt);
  self->setIor(self, ior);
  return (assoc);
}

MaterialIdAssoc	*transparentUnserializeSet(SceneSerializerClass *serializer,
                                   xmlNodePtr node,
                                   TransparentClass *self)
{
  MaterialIdAssoc	*assoc;
  char			*strKr, *strCr, *strKt, *strIor;
  float			kr, cr, kt, ior;

  if ((assoc = phongUnserializeSet(serializer, node, PHONG(self))) == 0)
    return (0);
  serializer->setStrsFromProp(serializer, node, "kr", &strKr, "cr",
      &strCr, "kt", &strKt, "ior", &strIor, NULL);
  if (setFloatFromStr(strKr, &kr) != 0
      || setFloatFromStr(strCr, &cr) != 0
      || setFloatFromStr(strKt, &kt) != 0
      || setFloatFromStr(strIor, &ior) != 0)
  {
    fprintf(stderr, "Error: bad attributes\n");
    return (0);
  }
  self->setKr(self, kr);
  self->setCr(self, cr);
  self->setKt(self, kt);
  self->setIor(self, ior);
  return (assoc);
}

static void	transparentSetCr(TransparentClass *self, float cr)
{
  PERFECTSPEC(self->private.mReflectiveBrdf)\
    ->setCr(PERFECTSPEC(self->private.mReflectiveBrdf), cr);
}

static void	transparentSetKr(TransparentClass *self, float kr)
{
  PERFECTSPEC(self->private.mReflectiveBrdf)\
    ->setKr(PERFECTSPEC(self->private.mReflectiveBrdf), kr);
}

static void	transparentSetKt(TransparentClass *self, float kt)
{
  self->private.mSpecularBtdf\
    ->setKt(self->private.mSpecularBtdf, kt);
}

static void	transparentSetIor(TransparentClass *self, float ior)
{
  self->private.mSpecularBtdf\
    ->setIor(self->private.mSpecularBtdf, ior);
}

static float		transparentShade(TransparentClass *self,
                                   HitRecordStruct *hitRecord,
                                   LightClass *currentLight)
{
  return (MATERIAL(Phong)->shade(self, hitRecord, currentLight));
}

static void	transparentShadeRGBCol(TransparentClass *self,
                                       HitRecordStruct *hitRecord,
                                       TransparentHit *c)
{
  c->ft = BTDFT(PRIV(mSpecularBtdf))\
    ->sampleF(BTDFT(PRIV(mSpecularBtdf)), hitRecord, &c->wo, &c->wt);
  c->transray = (RayStruct){hitRecord->hitPoint, c->wt};
  c->tmp = c->tracer->trace(c->tracer, &c->reflray,
    hitRecord->reflectDepth + 1);
  colorMultColor(&c->tmp, &c->fr);
  colorMult(&c->tmp, fabsf(vectDotProduct(&hitRecord->normal, &c->wi)));
  colorAdd(&c->color, &c->tmp);
  c->tmp = c->tracer->trace(c->tracer, &c->transray,
    hitRecord->reflectDepth + 1);
  colorMultColor(&c->tmp, &c->ft);
  colorMult(&c->tmp, fabsf(vectDotProduct(&hitRecord->normal, &c->wt)));
  colorAdd(&c->color, &c->tmp);
}

static Color		transparentShadeRGB(TransparentClass *self,
                                           HitRecordStruct *hitRecord)
{
  TransparentHit	c;

  c.color = MATERIAL(Phong)->shadeRGB(self, hitRecord);
  c.wo = hitRecord->ray.direction;
  vectOpTimes(&c.wo, -1.);
  c.fr = BRDFT(self->private.mReflectiveBrdf)
    ->sampleF(self->private.mReflectiveBrdf, hitRecord, &c.wo, &c.wi);
  c.reflray = (RayStruct){hitRecord->hitPoint, c.wi};
  c.tracer = WHITTED(TracerManagerGetTracer(eWhitted));
  if (hitRecord->inside)
    vectInv(&hitRecord->normal);
  if (BTDFT(PRIV(mSpecularBtdf))->tir(BTDFT(PRIV(mSpecularBtdf)), hitRecord))
  {
    c.tmp = c.tracer->trace(c.tracer, &c.reflray, hitRecord->reflectDepth + 1);
    colorAdd(&c.color, &c.tmp);
  }
  else
    transparentShadeRGBCol(self, hitRecord, &c);
  if (hitRecord->inside)
    vectInv(&hitRecord->normal);
  c.areaColor = MATERIAL(self)->shadeAreaLight(self, hitRecord);
  colorAdd(&c.color, &c.areaColor);
  return (c.color);
}

static Color		transparentShadePath(TransparentClass *self,
                                           HitRecordStruct *hitRecord)
{
  TransparentHit	c;

  c.wo = hitRecord->ray.direction;
  vectOpTimes(&c.wo, -1.);
  c.fr = BRDFT(self->private.mReflectiveBrdf)->sampleFPath(
    self->private.mReflectiveBrdf, hitRecord, &c.wo, &c.wi, &c.pdf);
  c.reflray = (RayStruct) { hitRecord->hitPoint, c.wi };
  c.ptracer = PATHTRACER(TracerManagerGetTracer(ePathTracer));
  if (hitRecord->inside)
    vectInv(&hitRecord->normal);
  if (!BTDFT(PRIV(mSpecularBtdf))->tir(BTDFT(PRIV(mSpecularBtdf)), hitRecord)
      && (rand() % 64) < 48)
  {
    c.ft = BTDFT(PRIV(mSpecularBtdf))\
      ->sampleF(BTDFT(PRIV(mSpecularBtdf)), hitRecord, &c.wo, &c.wt);
    c.reflray.direction = c.wt;
    c.fr.r = clamp(c.ft.r, 0, 1);
    c.fr.g = clamp(c.ft.g, 0, 1);
    c.fr.b = clamp(c.ft.b, 0, 1);
  }
  colorMultColor(&c.fr, COLORPTR(c.ptracer->trace(c.ptracer, &c.reflray,
    hitRecord->reflectDepth + 1)));
  if (hitRecord->inside)
    vectInv(&hitRecord->normal);
  colorMultColor(&c.fr, &(MATERIAL(self)->color));
  return (c.fr);
}

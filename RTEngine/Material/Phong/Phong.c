#include "RTTI.h"
#include "Phong.h"
#include "Lambertian.h"
#include "GlossySpecular.h"
#include "Utils.h"
#include "RenderingScene.h"
#include "AreaLight.h"
#include "PathTracer.h"
#include "TracerManager.h"

static void	phongConstructor(PhongClass *self, va_list *list);
static void	phongDestructor(PhongClass *self);
static void	phongSetKd(PhongClass *self, float kd);
static void	phongSetKs(PhongClass *self, float ks);
static float	phongShade(PhongClass *self,
                           HitRecordStruct *hitRecord,
                           LightClass *currentLight);
static void	phongSetExp(PhongClass *self, float exp);
static Color	phongShadeRGB(PhongClass *self, HitRecordStruct *hitRecord);
static void	phongShadeRGBLight(PhongClass *self,
                                   HitRecordStruct *hitRecord,
                                   Color *finalColor,
                                   LightClass *curLight);
static Color	phongShadeAreaLight(PhongClass *self,
                                    HitRecordStruct *hitRecord);
static Color	phongShadePath(PhongClass *self,
                               HitRecordStruct *hitRecord);

static void	phongShadeAreaLightSample(PhongClass *self,
                                          HitRecordStruct *hitRecord,
					  AreaLightClass *cur,
					  Color *color);

static PhongClass       _descr = { /* PhongClass */
  { /* MaterialClass */
    { /* Class */
      sizeof(PhongClass),
      __RTTI_PHONG, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
      &phongConstructor,
      &phongDestructor,
    },
    /* public */
    &phongShade, /* shade */
    &phongShadeRGB, /* shadeRGB */
    &phongShadeAreaLight,
    &phongShadePath, /* shadePath */
    /* public */
    {0., 0., 0.}, /* color */
    /* protected */
    {
      0, /* normalBuffer */
      0, /* pointBuffer */
      0 /* rayBuffer */
    }
  },
  0, /* phongSetKd */
  0, /* phongSetKs */
  0, /* phongSetExp */
  { /* private */
    0, /* mDiffuseBrdf */
    0 /* mGlossySpecularBrdf */
  }
};

Class *Phong = (Class*) &_descr;

static void	phongConstructor(PhongClass *self, va_list *list)
{
  Material->__constructor__(self, list);
  MATERIAL(self)->shade = &phongShade;
  MATERIAL(self)->shadeRGB = &phongShadeRGB;
  MATERIAL(self)->shadeAreaLight = &phongShadeAreaLight;
  MATERIAL(self)->shadePath = &phongShadePath;
  self->setKd = &phongSetKd;
  self->setKs = &phongSetKs;
  self->setExp = &phongSetExp;
  self->private.mDiffuseBrdf = new(Lambertian, eRegular);
  self->private.mGlossySpecularBrdf = new(GlossySpecular, eRegular);
}

static void	phongDestructor(PhongClass *self)
{
  delete(self->private.mDiffuseBrdf);
  delete(self->private.mGlossySpecularBrdf);
}

void	*phongUnserialize(SceneSerializerClass *serializer,
                           xmlNodePtr node)
{
  PhongClass		*self;
  MaterialIdAssoc	*assoc;
  char			*s[4];
  float			t[3];

  self = new(Phong);
  if (materialUnserializeSet(serializer, node, self) != 0)
    return (0);
  serializer->setStrsFromProp(serializer, node, "id", &s[0], "kd",
      &s[1], "ks", &s[2], "exp", &s[3], NULL);
  assoc = xmalloc(sizeof(*assoc));
  *assoc = (MaterialIdAssoc) {false, s[0], MATERIAL(self)};
  if (setFloatFromStr(s[1], &t[0]) != 0 || setFloatFromStr(s[2], &t[1]) != 0
      || setFloatFromStr(s[3], &t[2]) != 0)
  {
    fprintf(stderr, "Error: bad attributes\n");
    return (0);
  }
  free(s[1]);
  free(s[2]);
  free(s[3]);
  self->setKd(self, t[0]);
  self->setKs(self, t[1]);
  self->setExp(self, t[2]);
  return (assoc);
}

MaterialIdAssoc	*phongUnserializeSet(SceneSerializerClass *serializer,
                           xmlNodePtr node,
                           PhongClass *self)
{
  MaterialIdAssoc	*assoc;
  char			*str[4];
  float			t[3];

  if (materialUnserializeSet(serializer, node, self) != 0)
    return (0);
  serializer->setStrsFromProp(serializer, node, "id", &str[0], "kd",
      &str[1], "ks", &str[2], "exp", &str[3], NULL);
  assoc = xmalloc(sizeof(*assoc));
  *assoc = (MaterialIdAssoc) {false, str[0], MATERIAL(self)};
  if (setFloatFromStr(str[1], &t[0]) != 0
      || setFloatFromStr(str[2], &t[1]) != 0
      || setFloatFromStr(str[3], &t[2]) != 0)
  {
    fprintf(stderr, "Error: bad attributes\n");
    return (0);
  }
  free(str[1]);
  free(str[2]);
  free(str[3]);
  self->setKd(self, t[0]);
  self->setKs(self, t[1]);
  self->setExp(self, t[2]);
  return (assoc);
}

static void	phongSetKd(PhongClass *self, float kd)
{
  LAMBERTIAN(self->private.mDiffuseBrdf)\
    ->setKd(self->private.mDiffuseBrdf, kd);
}

static void	phongSetKs(PhongClass *self, float ks)
{
  GLOSSYSPEC(self->private.mGlossySpecularBrdf)\
    ->setKs(self->private.mGlossySpecularBrdf, ks);
}

static void	phongSetExp(PhongClass *self, float exp)
{
  GLOSSYSPEC(self->private.mGlossySpecularBrdf)\
    ->setExp(self->private.mGlossySpecularBrdf, exp);
}

static float		phongShade(PhongClass *self,
                                   HitRecordStruct *hitRecord,
                                   LightClass *currentLight)
{
  Vector3D		wo;
  Vector3D		wi;
  Vector3D		normal;
  float			nDotWi;
  float			l;

  wo = hitRecord->ray.direction;
  vectInv(&wo);
  normal = hitRecord->normal;
  wi = currentLight->getDirection(currentLight, hitRecord);
  vectInv(&wi);
  nDotWi = wi.x * normal.x + wi.y * normal.y + wi.z * normal.z;
  if ((nDotWi = -nDotWi) <= 0.)
    return (.0);
  if (RTTI(currentLight) == __RTTI_VPL)
    l = currentLight->ls * nDotWi
      * (BRDFT(self->private.mDiffuseBrdf)\
          ->f(self->private.mDiffuseBrdf, hitRecord, &wo, &wi));
  else
    l = currentLight->ls * nDotWi
      * (BRDFT(self->private.mDiffuseBrdf)\
          ->f(self->private.mDiffuseBrdf, hitRecord, &wo, &wi)
          + BRDFT(self->private.mGlossySpecularBrdf)\
          ->f(self->private.mGlossySpecularBrdf, hitRecord, &wo, &wi));
  return (l);
}

static void		phongShadeRGBLight(PhongClass *self,
                                      HitRecordStruct *hitRecord,
                                      Color *finalColor,
                                      LightClass *curLight)
{
  RayStruct		shadowRay;
  Vector3D		wi;
  Vector3D		wo;
  float			nDotWi;

  curLight->setHitRecordDist(curLight, hitRecord);
  shadowRay.direction = curLight->getDirection(curLight, hitRecord);
  shadowRay.origin = hitRecord->hitPoint;
  wi = shadowRay.direction;
  wo = hitRecord->ray.direction;
  if ((nDotWi = vectDotProduct(&wi, &hitRecord->normal)) < 0.f)
    return ;
  if (!curLight->inShadow(curLight, &shadowRay))
  {
    Color tmp = MATERIAL(self)->color;
    colorMult(&tmp, BRDFT(self->private.mDiffuseBrdf)\
          ->f(self->private.mDiffuseBrdf, hitRecord, &wo, &wi));
    colorAddFloat(&tmp, BRDFT(self->private.mGlossySpecularBrdf)\
          ->f(self->private.mGlossySpecularBrdf, hitRecord, &wo, &wi));
    colorMultColor(&tmp, COLORPTR(curLight->l(curLight, hitRecord)));
    colorMult(&tmp, nDotWi);
    colorAdd(finalColor, &tmp);
  }
}

static Color		phongShadeRGB(PhongClass *self,
                                      HitRecordStruct *hitRecord)
{
  int			i;
  Color			color;
  Color			areaColor;
  SceneClass		*scene;
  LightClass		*cur;

  i = 0;
  scene = RenderingSceneGetSingletonPtr()->getCurrentScene();
  cur = LIGHT(scene->getAmbientPtr(scene));
  color = MATERIAL(self)->color;
  colorMultColor(&color, COLORPTR(cur->l(cur, hitRecord)));
  while (i < scene->mLights->size(scene->mLights))
  {
    cur = scene->mLights->mPtrs[i];
    phongShadeRGBLight(self, hitRecord, &color, cur);
    i++;
  }
  areaColor = phongShadeAreaLight(self, hitRecord);
  colorAdd(&color, &areaColor);
  return (color);
}

static void		phongShadeAreaLightSample(PhongClass *self,
                                                  HitRecordStruct *hitRecord,
						  AreaLightClass *cur,
						  Color *color)
{
  Color			addedColor;
  int			i;

  i = 0;
  while (i < cur->samples)
  {
    addedColor = (Color){0, 0, 0};
    phongShadeRGBLight(self, hitRecord, &addedColor, LIGHT(cur));
    colorMult(&addedColor, cur->g(cur, hitRecord)
              / (cur->pdf(cur, hitRecord) * cur->samples));
    colorAdd(color, &addedColor);
    ++i;
  }
}

static Color		phongShadePath(PhongClass *self,
                                       HitRecordStruct *hitRecord)
{
  Vector3D		wi;
  Vector3D		wo;
  float			pdf;
  Color			fr;
  RayStruct		reflray;

  wo = hitRecord->ray.direction;
  vectInv(&wo);
  if ((rand() % 64) > 48)
    fr = BRDFT(PRIV(mGlossySpecularBrdf))->sampleFPath(PRIV(mGlossySpecularBrdf),
                                                hitRecord, &wo, &wi, &pdf);
  else
    fr = BRDFT(PRIV(mDiffuseBrdf))->sampleFPath(PRIV(mDiffuseBrdf),
                                                hitRecord, &wo, &wi, &pdf);
  colorMultColor(&fr, &MATERIAL(self)->color);
  reflray = (RayStruct){hitRecord->hitPoint, wi};
  colorMultColor(&fr, COLORPTR(PATHTRACER(TracerManagerGetTracer(ePathTracer))\
      ->trace(PATHTRACER(TracerManagerGetTracer(ePathTracer)),
        &reflray,
        hitRecord->reflectDepth + 1)));
  colorMult(&fr, vectDotProduct(&hitRecord->normal, &wi) / pdf);
  return (fr);
}

static Color		phongShadeAreaLight(PhongClass *self,
                                            HitRecordStruct *hitRecord)
{
  Color			color;
  LightClass		*cur;
  SceneClass		*scene;
  int			i;

  scene = RenderingSceneGetSingletonPtr()->getCurrentScene();
  color = (Color){0, 0, 0};
  if ((cur = LIGHT(scene->getEnvLightPtr(scene))))
    phongShadeAreaLightSample(self, hitRecord, AREALIGHT(cur), &color);
  colorMultColor(&color, &MATERIAL(self)->color);
  i = 0;
  while (i < scene->mAreaLights->size(scene->mAreaLights))
  {
    cur = scene->mAreaLights->mPtrs[i];
    phongShadeAreaLightSample(self, hitRecord, AREALIGHT(cur), &color);
    i++;
  }
  return (color);
}

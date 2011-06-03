#include "RTTI.h"
#include "Matte.h"
#include "Lambertian.h"
#include "HitRecord.h"
#include "SceneSerializer.h"
#include "Utils.h"
#include "RenderingScene.h"
#include "AreaLight.h"
#include "TracerManager.h"
#include "PathTracer.h"

static void	matteConstructor(MatteClass *self, va_list *list);
static void	matteDestructor(MatteClass *self);
static void	matteSetKd(MatteClass *self, float kd);
static float	matteShade(MatteClass *self, HitRecordStruct *hitRecord,
                                   LightClass *currentLight);
static void	matteShadeRGBLight(MatteClass *self,
                                   HitRecordStruct *hitRecord,
                                   Color *finalColor,
                                   LightClass *curLight);
static Color	matteShadeRGB(MatteClass *self,
                                      HitRecordStruct *hitRecord);
static void	matteShadeAreaLightSample(MatteClass *self,
                                          HitRecordStruct *hitRecord,
					  AreaLightClass *cur,
					  Color *color);
static Color	matteShadeAreaLight(MatteClass *self,
                                    HitRecordStruct *hitRecord);
static Color	matteShadePath(MatteClass *self,
                               HitRecordStruct *hitRecord);

static MatteClass       _descr = { /* MatteClass */
    { /* MaterialClass */
      { /* Class */
        sizeof(MatteClass),
        __RTTI_MATTE, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
        &matteConstructor,
        &matteDestructor,
      },
      /* public */
      &matteShade, /* shade */
      &matteShadeRGB,
      &matteShadeAreaLight,
      &matteShadePath,
      /* public */
      {0., 0., 0.}, /* color */
      /* protected */
      {
        0, /* normalBuffer */
        0, /* pointBuffer */
        0 /* rayBuffer */
      }
    },
    &matteSetKd,
    { /* private */
      0 /* mDiffuseBrdf */
    }
  };

Class *Matte = (Class*) &_descr;

static void	matteConstructor(MatteClass *self, va_list *list)
{
  Material->__constructor__(self, list);
  self->private.mDiffuseBrdf = new(Lambertian, eRegular);
}

void	*matteUnserialize(SceneSerializerClass *serializer,
                           xmlNodePtr node)
{
  MatteClass		*self;
  MaterialIdAssoc	*assoc;
  char			*strId, *strKd;
  float			kd;

  self = new(Matte);
  if (materialUnserializeSet(serializer, node, self) != 0)
    return (0);
  serializer->setStrsFromProp(serializer, node, "id", &strId, "kd",
      &strKd, NULL);
  assoc = xmalloc(sizeof(*assoc));
  *assoc = (MaterialIdAssoc) {false, strId, MATERIAL(self)};
  if (setFloatFromStr(strKd, &kd) != 0)
  {
    fprintf(stderr, "Error: bad attributes\n");
    return (0);
  }
  self->setKd(self, kd);
  return (assoc);
}

static void	matteDestructor(MatteClass *self)
{
  delete(self->private.mDiffuseBrdf);
}

static void	matteSetKd(MatteClass *self, float kd)
{
  LAMBERTIAN(self->private.mDiffuseBrdf)\
    ->setKd(self->private.mDiffuseBrdf, kd);
}

static float		matteShade(MatteClass *self, HitRecordStruct *hitRecord,
                                   LightClass *currentLight)
{
  Vector3D		wo;
  Vector3D		wi;
  Vector3D		normal;
  float			nDotWi;
  float			l;

  wo = hitRecord->ray.direction;
  wo = (Vector3D){-wo.x, -wo.y, -wo.z};
  normal = hitRecord->normal;
  wi = currentLight->getDirection(currentLight, hitRecord);
  wi = (Vector3D){-wi.x, -wi.y, -wi.z};
  nDotWi = wi.x * normal.x + wi.y * normal.y + wi.z * normal.z;
  nDotWi = -nDotWi;
  if (nDotWi <= .0)
    return (.0);
  l = currentLight->ls;
  l = l * nDotWi * BRDFT(self->private.mDiffuseBrdf)\
      ->f(self->private.mDiffuseBrdf, hitRecord, &wo, &wi);
  return (l);
}

/* vvv */

static void	matteShadeRGBLight(MatteClass *self,
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
    colorMultColor(&tmp, COLORPTR(curLight->l(curLight, hitRecord)));
    colorMult(&tmp, nDotWi);
    colorAdd(finalColor, &tmp);
  }
}

static Color		matteShadeRGB(MatteClass *self,
                                      HitRecordStruct *hitRecord)
{
  Color			color;
  Color			areaColor;
  LightClass		*cur;
  SceneClass		*scene;
  int			i;

  scene = RenderingSceneGetSingletonPtr()->getCurrentScene();
  cur = LIGHT(scene->getAmbientPtr(scene));
  color = MATERIAL(self)->color;
  colorMultColor(&color, COLORPTR(cur->l(cur, hitRecord)));
  i = 0;
  while (i < scene->mLights->size(scene->mLights))
  {
    cur = scene->mLights->mPtrs[i];
    matteShadeRGBLight(self, hitRecord, &color, cur);
    i++;
  }
  areaColor = matteShadeAreaLight(self, hitRecord);
  colorAdd(&color, &areaColor);
  return (color);
}

/* ^^^ */

static void		matteShadeAreaLightSample(MatteClass *self,
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
    matteShadeRGBLight(self, hitRecord, &addedColor, LIGHT(cur));
    colorMult(&addedColor, cur->g(cur, hitRecord)
              / (cur->pdf(cur, hitRecord) * cur->samples));
    colorAdd(color, &addedColor);
    ++i;
  }
}

static Color		matteShadeAreaLight(MatteClass *self,
                                            HitRecordStruct *hitRecord)
{
  Color			color;
  LightClass		*cur;
  SceneClass		*scene;
  int			i;

  scene = RenderingSceneGetSingletonPtr()->getCurrentScene();
  color = (Color){0, 0, 0};
  if ((cur = LIGHT(scene->getEnvLightPtr(scene))))
    matteShadeAreaLightSample(self, hitRecord, AREALIGHT(cur), &color);
  colorMultColor(&color, &MATERIAL(self)->color);
  i = 0;
  while (i < scene->mAreaLights->size(scene->mAreaLights))
  {
    cur = scene->mAreaLights->mPtrs[i];
    matteShadeAreaLightSample(self, hitRecord, AREALIGHT(cur), &color);
    i++;
  }
  return (color);
}

static Color		matteShadePath(MatteClass *self,
                                       HitRecordStruct *hitRecord)
{
  Vector3D		wi;
  Vector3D		wo;
  float			pdf;
  Color			fr;
  float			nDotWi;

  wo  = hitRecord->ray.direction;
  vectInv(&wo);
  fr = BRDFT(PRIV(mDiffuseBrdf))->sampleFPath(PRIV(mDiffuseBrdf),
                                              hitRecord, &wo, &wi, &pdf);
  colorMultColor(&fr, &MATERIAL(self)->color);
  nDotWi = vectDotProduct(&hitRecord->normal, &wi);
#ifdef FASTPT
  if ((rand() % 64) > 58)
  {
    colorMult(&fr, nDotWi / pdf);
    return (fr);
  }
#endif
  RayStruct		reflectedRay = {hitRecord->hitPoint, wi};
  colorMultColor(&fr, COLORPTR(PATHTRACER(TracerManagerGetTracer(ePathTracer))\
      ->trace(PATHTRACER(TracerManagerGetTracer(ePathTracer)),
        &reflectedRay,
        hitRecord->reflectDepth + 1)));
  colorMult(&fr, nDotWi / pdf);
  return (fr);
}

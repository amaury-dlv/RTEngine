#include <math.h>
#include <omp.h>

#include "AreaLight.h"
#include "HitRecord.h"
#include "RenderingScene.h"
#include "Maths.h"
#include "SceneSerializer.h"
#include "Emissive.h"

static void     areaLightConstructor(AreaLightClass* self, va_list* list);
static void     areaLightDestructor(AreaLightClass*self);
static Color	areaLightL(AreaLightClass *self, HitRecordStruct *hitRecord);
static float	areaLightG(AreaLightClass *self, HitRecordStruct *hitRecord);
static float	areaLightPdf(AreaLightClass *self, HitRecordStruct *hitRecord);
static Vector3D	areaLightGetDirection(AreaLightClass *self,
                                      HitRecordStruct *hitRecord);
static bool	areaLightInShadow(AreaLightClass *self, RayStruct *shadowRay);
static Vector3D	areaLightGetSampleThreadsafe(AreaLightClass *self);

static AreaLightClass       _descr = { /* AreaLightClass */
    { /* LightClass */
      { /* Class */
        sizeof(AreaLightClass),
        __RTTI_AREALIGHT,
        &areaLightConstructor,
        &areaLightDestructor,
      },
      &areaLightGetDirection, /* getDirection() */
      &areaLightL, /* l() */
      VOID(&areaLightInShadow), /* inShadow() */
      0, /* lightSetHitRecordDist */
      0, /* id */
      1., /* ls */
      {1, 1, 1}, /* color */
    },
    &areaLightG,
    &areaLightPdf,
    1, /* mSamples */
    { /* private */
      0, /* mObject */
      0, /* mMaterial */
      {0, 0, 0}, /* samplePoint */
      {
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0}
      }, /* samplePoint */
      {0, 0, 0}, /* normal */
      {0, 0, 0} /* wi */
    }
  };

Class* AreaLight = (Class*) &_descr;

static void     	areaLightConstructor(AreaLightClass *self,
                                             va_list *list)
{
  Light->__constructor__(self, list);
  PRIV(mObject) = va_arg(*list, void*);
  PRIV(mMaterial) = va_arg(*list, void*);
}

static void     areaLightDestructor(AreaLightClass *self)
{
  Light->__destructor__(self);
}

static Color		areaLightL(AreaLightClass *self,
                                   HitRecordStruct *hitRecord)
{
  Vector3D	normal;

  normal = PRIV(normal);
  vectInv(&normal);
  if (vectDotProduct(&normal, &PRIV(wi)) > 0.)
    return (EMISSIVE(PRIV(mMaterial))->getLe(EMISSIVE(PRIV(mMaterial))));
  return ((Color){0, 0, 0});
}

static Vector3D	areaLightGetSampleThreadsafe(AreaLightClass *self)
{
#ifdef USE_THREADS
  int			curThread;

  curThread = omp_get_thread_num();
  PRIV(samplePoint1)[curThread] = PRIV(mObject)->sample(PRIV(mObject));
  return (PRIV(samplePoint1)[curThread]);
#else
  PRIV(samplePoint) = PRIV(mObject)->sample(PRIV(mObject));
  return (PRIV(samplePoint));
#endif
}

static Vector3D	areaLightGetDirection(AreaLightClass *self,
                                      HitRecordStruct *hitRecord)
{
  Vector3D	samplePoint;

  samplePoint = areaLightGetSampleThreadsafe(self);
  PRIV(normal) = PRIV(mObject)->getNormal(PRIV(mObject), &samplePoint);
  PRIV(wi) = samplePoint;
  vectOpMinus(&PRIV(wi), &hitRecord->hitPoint);
  vectNormalize(&PRIV(wi));
  return (PRIV(wi));
}

static float		areaLightG(AreaLightClass *self,
                                   HitRecordStruct *hitRecord)
{
  Vector3D		normal;
  Vector3D		samplePoint;
  float			nDotD;
  float			d2;

  normal = PRIV(normal);
  vectInv(&normal);
  nDotD = vectDotProduct(&normal, &PRIV(wi));
  d2 = vectDist(&samplePoint, &hitRecord->hitPoint);
  d2 *= d2;
  return (nDotD / d2);
}

static float		areaLightPdf(AreaLightClass *self,
                                     HitRecordStruct *hitRecord)
{
  return (PRIV(mObject)->pdf(PRIV(mObject), hitRecord));
}

static bool		areaLightInShadow(AreaLightClass *self,
                                          RayStruct *shadowRay)
{
  SceneClass		*scene;
  GeometricObjectClass	*obj;
  RayStruct		ray;
  double		d;
  double		t;
  int			i;
  Vector3D		tmp;

  tmp = areaLightGetSampleThreadsafe(self);
  ray = *shadowRay;
  vectOpMinus(&tmp, &shadowRay->origin);
  d = vectDotProduct(&tmp, &shadowRay->direction);
  i = 0;
  t = Infinity;
  scene = RenderingSceneGetSingletonPtr()->getCurrentScene();
  while (i < scene->mObjects->size(scene->mObjects))
  {
    obj = scene->mObjects->mPtrs[i];
    if (obj->hit(obj, &ray, &t) && t < (d - kEpsilon) && t > ShadowKEpsilon
        && obj->mId != PRIV(mObject)->mId)
      return (true);
    i++;
  }
  return (false);
}

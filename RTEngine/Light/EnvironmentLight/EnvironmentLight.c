#include <math.h>

#include "EnvironmentLight.h"
#include "HitRecord.h"
#include "RenderingScene.h"
#include "Maths.h"
#include "SceneSerializer.h"
#include "Emissive.h"
#include "SamplerManager.h"
#include "RTEngine.h"

static void     environmentLightConstructor();
static void     environmentLightDestructor(EnvironmentLightClass*self);
static Color	environmentLightL(EnvironmentLightClass *self,
                                  HitRecordStruct *hitRecord);
static float	environmentLightG(EnvironmentLightClass *self,
                                  HitRecordStruct *hitRecord);
static float	environmentLightPdf(EnvironmentLightClass *self,
                                    HitRecordStruct *hitRecord);
static Vector3D	environmentLightGetDirection(EnvironmentLightClass *self,
                                             HitRecordStruct *hitRecord);
static bool	environmentLightInShadow(EnvironmentLightClass *self,
                                         RayStruct *shadowRay);

static EnvironmentLightClass       _descr = { /* EnvironmentLightClass */
    { /* LightClass */
      { /* Class */
        sizeof(EnvironmentLightClass),
        __RTTI_ENVIRONMENTLIGHT, /* RTTI */
        &environmentLightConstructor,
        &environmentLightDestructor,
      },
      &environmentLightGetDirection, /* getDirection() */
      &environmentLightL, /* l() */
      VOID(&environmentLightInShadow), /* inShadow() */
      0, /* lightSetHitRecordDist */
      0, /* id */
      1., /* ls */
      {1, 1, 1}, /* color */
    },
    &environmentLightG,
    &environmentLightPdf,
    1, /* mSamples */
    { /* private */
      0, /* mSampler */
      0, /* mMaterial */
      {0, 0, 0}, /* u */
      {0, 0, 0}, /* v */
      {0, 0, 0}, /* w */
      {0, 0, 0} /* wi */
    }
  };

Class* EnvironmentLight = (Class*) &_descr;

static void     environmentLightConstructor(EnvironmentLightClass *self,
                                            va_list *list)
{
  Light->__constructor__(self, list);
  PRIV(mMaterial) = va_arg(*list, void*);
  PRIV(mSampler) =
    SamplerManagerGetSingletonPtr()->getSampler(eMultiJittered,
      RTEngineGetSingletonPtr()->getDefaultNumSamples(),
      RTEngineGetSingletonPtr()->getDefaultNumSets(),
      (float)100.);
  PRIV(mSampler)->mapSamplesToHemisphere(PRIV(mSampler), (float)1.);
}

static void     environmentLightDestructor(EnvironmentLightClass *self)
{
  Light->__destructor__(self);
}

void		*environmentLightUnserialize(SceneSerializerClass *serializer,
                                    xmlNodePtr node)
{
  EnvironmentLightClass	*self;
  MaterialClass		*mat;
  char			*strIdMat;

  serializer->setStrsFromProp(serializer, node, "id_mat", &strIdMat, NULL);
  if (!(mat = serializer->getMaterialFromId(serializer, strIdMat)))
  {
    fprintf(stderr, "Error: bad attributes\n");
    return (0);
  }
  self = new(EnvironmentLight, mat);
  if (lightUnserializeSet(serializer, node, self) != 0)
    return (0);
  return (self);
}

static Color		environmentLightL(EnvironmentLightClass *self,
                                          HitRecordStruct *hitRecord)
{
  return (EMISSIVE(PRIV(mMaterial))->getLe(EMISSIVE(PRIV(mMaterial))));
}

static Vector3D	environmentLightGetDirection(EnvironmentLightClass *self,
                                             HitRecordStruct *hitRecord)
{
  Point3D		sample;
  Point3D		tmp;

  PRIV(w) = hitRecord->normal;
  PRIV(v) = (Vector3D){0.0034, 1, 0.0071};
  vectCrossProduct(&PRIV(v), &PRIV(w));
  vectNormalize(&PRIV(v));
  PRIV(u) = PRIV(v);
  vectCrossProduct(&PRIV(u), &PRIV(w));
  sample = PRIV(mSampler)->nextHemisphereSample(PRIV(mSampler));
  tmp = PRIV(u);
  vectOpTimes(&tmp, sample.x);
  PRIV(wi) = tmp;
  tmp = PRIV(v);
  vectOpTimes(&tmp, sample.y);
  vectOpPlus(&PRIV(wi), &tmp);
  tmp = PRIV(w);
  vectOpTimes(&tmp, sample.z);
  vectOpPlus(&PRIV(wi), &tmp);
  return (PRIV(wi));
}

static float		environmentLightG(EnvironmentLightClass *self,
                                          HitRecordStruct *hitRecord)
{
  return (1);
}

static float		environmentLightPdf(EnvironmentLightClass *self,
                                            HitRecordStruct *hitRecord)
{
  return (1);
}

static bool		environmentLightInShadow(EnvironmentLightClass *self,
                                                 RayStruct *shadowRay)
{
  SceneClass		*scene;
  GeometricObjectClass	*obj;
  RayStruct		ray;
  double		t;
  double		tMin;
  int			i;

  ray = *shadowRay;
  i = 0;
  t = tMin = Infinity;
  scene = RenderingSceneGetSingletonPtr()->getCurrentScene();
  while (i < scene->mObjects->size(scene->mObjects))
  {
    obj = scene->mObjects->mPtrs[i];
    if (obj->hit(obj, &ray, &t) && t < tMin && t > ShadowKEpsilon)
      return (true);
    i++;
  }
  return (false);
}

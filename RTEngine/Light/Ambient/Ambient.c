#include "Ambient.h"
#include "HitRecord.h"
#include "SceneSerializer.h"
#include "Maths.h"
#include "SamplerManager.h"
#include "RTEngine.h"

static void     ambientConstructor(AmbientClass *self, va_list *list);
static void     ambientDestructor(AmbientClass*self);
static Color	ambientL(AmbientClass *self, HitRecordStruct *hitRecord);
static Vector3D	ambientGetDirection(AmbientClass *self,
                                    HitRecordStruct *hitRecord);
static bool	ambientInShadow(AmbientClass *self, RayStruct *shadowRay);
static void	ambientSetOccluder(AmbientClass *self,
                                   int samples, float occlusion);
static void	ambientLSample(AmbientClass *self, HitRecordStruct *hitRecord,
                               RayStruct *shadowRay, Color *color);

static AmbientClass       _descr = { /* AmbientClass */
  { /* LightClass */
    { /* Class */
      sizeof(AmbientClass),
      __RTTI_AMBIENT, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
      &ambientConstructor,
      &ambientDestructor,
    },
    &ambientGetDirection, /* getDirection() */
    &ambientL, /* l() */
    VOID(&ambientInShadow), /* inShadow() */
    0, /* lightSetHitRecordDist */
    0, /* mId */
    0., /* mLs */
    {0., 0., 0.}, /* mColor */
  },
  &ambientSetOccluder, /* setOccluder() */
  { /* private */
    false, /* mIsOccluder */
    {0, 0, 0}, /* u */
    {0, 0, 0}, /* v */
    {0, 0, 0}, /* w */
    0, /* mSampler */
    25, /* mSamples */
    0.1 /* mOcclusionFactor */
  }
};

Class *Ambient = (Class*) &_descr;

static void     ambientConstructor(AmbientClass *self, va_list *list)
{
  Light->__constructor__(self, list);
}

static void     ambientDestructor(AmbientClass *self)
{
  Light->__destructor__(self);
}

void		*ambientUnserialize(SceneSerializerClass *serializer,
                                    xmlNodePtr node)
{
  AmbientClass	*self;

  self = new(Ambient);
  if (lightUnserializeSet(serializer, node, self) != 0)
    return (0);
  return (self);
}

static void	ambientLSample(AmbientClass *self, HitRecordStruct *hitRecord,
                               RayStruct *shadowRay, Color *color)
{
  Color		tmp;
  Color		nonOccluded;
  int		i;

  nonOccluded = (Color){LIGHT(self)->ls * LIGHT(self)->color.r,
                  LIGHT(self)->ls * LIGHT(self)->color.g,
                  LIGHT(self)->ls * LIGHT(self)->color.b};
  i = 0;
  while (i < PRIV(mSamples))
  {
    shadowRay->direction = ambientGetDirection(self, hitRecord);
    tmp = nonOccluded;
    tmp.r = 1;
    if (ambientInShadow(self, shadowRay))
      colorMult(&tmp, PRIV(mOcclusionFactor));
    colorAdd(color, &tmp);
    ++i;
  }
  colorMult(color, 1.0f / i);
}

static Color	ambientL(AmbientClass *self, HitRecordStruct *hitRecord)
{
  LightClass	*selfLight;
  Color		color;
  RayStruct	shadowRay;

  if (!PRIV(mIsOccluder))
  {
    selfLight = (LightClass *) self;
    return ((Color){selfLight->ls, selfLight->ls, selfLight->ls});
  }
  PRIV(w) = hitRecord->normal;
  PRIV(v) = PRIV(w);
  PRIV(u) = (Vector3D){0.0072, 1.0, 0.0034};
  vectCrossProduct(&PRIV(v), &PRIV(u));
  vectNormalize(&PRIV(v));
  PRIV(u) = PRIV(v);
  vectCrossProduct(&PRIV(u), &PRIV(w));
  shadowRay.origin = hitRecord->hitPoint;
  color = (Color){0, 0, 0};
  ambientLSample(self, hitRecord, &shadowRay, &color);
  return (color);
}

static Vector3D	ambientGetDirection(AmbientClass *self,
                                    HitRecordStruct *hitRecord)
{
  Vector3D	sample;
  Vector3D	res;
  Vector3D	tmp;

  if (!PRIV(mIsOccluder))
    return ((Vector3D){0, 0, 0});
  sample = PRIV(mSampler)->nextHemisphereSample(PRIV(mSampler));
  tmp = PRIV(u);
  vectOpTimes(&tmp, sample.x);
  res = tmp;
  tmp = PRIV(v);
  vectOpTimes(&tmp, sample.y);
  vectOpPlus(&res, &tmp);
  tmp = PRIV(w);
  vectOpTimes(&tmp, sample.z);
  vectOpPlus(&res, &tmp);
  return (res);
}

static bool		ambientInShadow(AmbientClass *self,
                                        RayStruct *shadowRay)
{
  SceneClass		*scene;
  GeometricObjectClass	*obj;
  RayStruct		ray;
  double		t;
  int			i;

  if (!PRIV(mIsOccluder))
    return (false);
  ray = *shadowRay;
  i = 0;
  t = Infinity;
  scene = RenderingSceneGetSingletonPtr()->getCurrentScene();
  while (i < scene->mObjects->size(scene->mObjects))
  {
    obj = scene->mObjects->mPtrs[i];
    if (obj->hit(obj, &ray, &t) && t < Infinity && t > ShadowKEpsilon)
      return (true);
    i++;
  }
  return (false);
}

static void	ambientSetOccluder(AmbientClass *self, int samples,
                                   float occlusion)
{
  PRIV(mSampler) =
    SamplerManagerGetSingletonPtr()->getSampler(eJittered, samples,
      RTEngineGetSingletonPtr()->getDefaultNumSets(), (float)100.);
  PRIV(mSampler)->mapSamplesToHemisphere(PRIV(mSampler), (float)1.);
  PRIV(mSamples) = samples;
  PRIV(mOcclusionFactor) = occlusion;
  PRIV(mIsOccluder) = true;
}

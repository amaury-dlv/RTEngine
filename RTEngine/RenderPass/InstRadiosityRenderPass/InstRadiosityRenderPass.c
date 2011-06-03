#include "InstRadiosityRenderPass.h"
#include "Color.h"
#include "RenderingScene.h"
#include "RTEngine.h"
#include "Maths.h"
#include "PointLight.h"
#include "TracerManager.h"
#include "Raycast.h"
#include "Sphere.h"
#include "Utils.h"
#include "SceneSerializer.h"

static void     InstRadiosityConstructor();
static void     InstRadiosityDestructor(InstRadiosityClass *self);
void		*instRadiosityRenderPassUnserialize(
    SceneSerializerClass *serializer,
    xmlNodePtr node);
static void	instRadiosityInit(InstRadiosityClass *self);
static void	instRadiosityAddVPL(InstRadiosityClass *self,
                                            HitRecordStruct *hit,
                                            LightClass *originalLight,
                                            int iBounce);
static void	instRadiosityMapVPLColor(InstRadiosityClass *self,
                                         PointLightClass *curLight,
                                         Color *toAdd);
static void	instRadiosityAddVPLColorToBuffer(
  InstRadiosityClass *self,
  PointLightClass *curLight,
  int i);
static void	instRadiosityApplyVPL(InstRadiosityClass *self,
                                      PointLightClass *light);
static void	instRadiosityCreateFirstBounceVPLs(InstRadiosityClass *self,
    						   PointLightClass *light);
static void	instRadiositySetBounceDirection(InstRadiosityClass *self,
                                                Vector3D *direction,
                                                HitRecordStruct *hitRec);
static void	instRadiosityCreateBounceFromVPL(
  InstRadiosityClass *self,
  PointLightClass *curLight,
  HitRecordStruct *lastHit);
static void	instRadiosityApplyLoopCreate(InstRadiosityClass *self);
static void	instRadiosityApplyLoopSet(InstRadiosityClass *self);
static void	instRadiosityApplyLoop(InstRadiosityClass *self);
static void	instRadiosityDisplayStep(InstRadiosityClass *self);
static void	instRadiosityUpdateCurrentState(InstRadiosityClass *self);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static InstRadiosityClass       _descr = { /* InstRadiosityClass */
  { /* RenderPassClass */
    { /* Class */
      sizeof(InstRadiosityClass),
      __RTTI_INSTRADIOSITYRENDERPASS, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
      &InstRadiosityConstructor,
      &InstRadiosityDestructor,
    },
    &instRadiosityInit,
    0, /* apply */
    VOID(&instRadiosityApplyLoop), /* applyLoop */
    0,
    0,
    VOID(&instRadiosityUpdateCurrentState),
  },
  { /* private */
    0, /* mVPLBuffer */
    0, /* mScene */
    0, /* mLightVect */
    0, /* mFirstBounceSampler */
    0, /* mVPLVect */
    0, /* mHitRecVect */
    0, /* mCurrentState */
    0, /* mMaterialBuffer */
    0, /* mStepBuffer */
    0, /* mDisplayBuffer */
    0, /* mSpheresVec */
    0, /* hasBounce */
    0.1, /* mThreshold */
    10, /* mMapPower */
    1 /* mMapFactor */
  }
};

Class *InstRadiosity = (Class*) &_descr;

static void     InstRadiosityConstructor(InstRadiosityClass *self,
                                         va_list *list)
{
  int		width;
  int		height;

  RenderPass->__constructor__(self, list);
  self->private.mHasBounce = va_arg(*list, int);
  self->private.mFirstBounceSampler = SamplerManagerGetSingletonPtr()\
    ->getSampler(eJittered, va_arg(*list, int), 83, (float)100.);
  width = RTEngineGetSingletonPtr()->getSceneWidth();
  height = RTEngineGetSingletonPtr()->getSceneHeight();
  self->private.mVPLBuffer = new(ColorBuffer, width, height);
  self->private.mVPLVect = new(Vector);
  self->private.mHitRecVect = new(Vector);
  self->private.mStepBuffer = new(ColorBuffer, width, height);
  self->private.mDisplayedBuffer = new(ColorBuffer, width, height);
  self->private.mSpheresVec = new(Vector);
  RENDERPASS(self)->applyLoop = VOID(&instRadiosityApplyLoop);
}

static void     InstRadiosityDestructor(InstRadiosityClass *self)
{
  RenderPass->__destructor__(self);
}

void		*instRadiosityRenderPassUnserialize(
    SceneSerializerClass *serializer,
    xmlNodePtr node)
{
  InstRadiosityClass	*self;
  char		*strHasBounce;
  char		*strNumVpls;
  int		hasBounce;
  int		numVpls;
  char		*strMapping;

  serializer->setStrsFromProp(serializer, node, "bounce", &strHasBounce,
      "vpl", &strNumVpls, NULL);
  if (setIntFromStr(strHasBounce, &hasBounce) != 0
      || setIntFromStr(strNumVpls, &numVpls) != 0)
    return (0);
  self = new(InstRadiosity, !!hasBounce, numVpls);
  if (((strMapping = (char*)xmlGetProp(node, XMLSTR("threshold")))
        && (setFloatFromStr(strMapping, &PRIV(mThreshold))))
      || ((strMapping = (char*)xmlGetProp(node, XMLSTR("mapPower")))
        && (setFloatFromStr(strMapping, &PRIV(mMapPower))))
      || ((strMapping = (char*)xmlGetProp(node, XMLSTR("mapFactor")))
        && (setFloatFromStr(strMapping, &PRIV(mMapFactor)))))
    return (0);
  if (renderPassUnserializeSet(serializer, node, self) != 0)
    return (0);
  return (self);
}

static void	instRadiosityInit(InstRadiosityClass *self)
{
  GBufferClass 	*gBuffer;

  self->private.mScene = RenderingSceneGetSingletonPtr()\
                          ->getCurrentScene();
  self->private.mLightVect = self->private.mScene->mLights;
  self->private.mCurrentState
    = RenderingSceneGetSingletonPtr()->getCurrentState();
  gBuffer = RenderingSceneGetSingletonPtr()->getGBuffer();
  self->private.mMaterialBuffer =
    MATERIALBUF(gBuffer->getBuffer(gBuffer, RTTI(MaterialBuffer)));
}

static void		instRadiosityAddVPL(InstRadiosityClass *self,
                                            HitRecordStruct *hit,
                                            LightClass *originalLight,
                                            int iBounce)
{
  PointLightClass	*vpl;

  vpl = new(PointLight);
  vpl->mPos = hit->hitPoint;
  LIGHT(vpl)->color = hit->material->color;
  RTTI(vpl) = __RTTI_VPL;
  if (iBounce == 0)
  {
    LIGHT(vpl)->ls = (LIGHT(originalLight)->ls * 4 * Pi)
      / ((float) self->private.mFirstBounceSampler\
         ->getNumSamples(self->private.mFirstBounceSampler));
    self->private.mHitRecVect->push_back(self->private.mHitRecVect,
                                         hitRecordCopy(hit));
  }
  else
    LIGHT(vpl)->ls = LIGHT(originalLight)->ls;
  self->private.mVPLVect->push_back(self->private.mVPLVect, vpl);
}

static void	instRadiosityMapVPLColor(InstRadiosityClass *self,
                                         PointLightClass *curLight,
                                         Color *toAdd)
{
  float		light_threshold;

  light_threshold = PRIV(mThreshold) * LIGHT(curLight)->ls;
  toAdd->r = toAdd->r / (toAdd->r + PRIV(mMapPower)) * PRIV(mMapFactor);
  toAdd->g = toAdd->g / (toAdd->g + PRIV(mMapPower)) * PRIV(mMapFactor);
  toAdd->b = toAdd->b / (toAdd->b + PRIV(mMapPower)) * PRIV(mMapFactor);
  if (toAdd->r > light_threshold)
    toAdd->r = light_threshold;
  if (toAdd->g > light_threshold)
    toAdd->g = light_threshold;
  if (toAdd->b > light_threshold)
    toAdd->b = light_threshold;
}

static void		instRadiosityAddVPLColorToBuffer(
  InstRadiosityClass *self,
  PointLightClass *curLight,
  int i)
{
  MaterialClass		*material;
  HitRecordStruct	hitRecord;
  float			k;
  Color			toAdd;
  Color			*color;

  material = PRIV(mMaterialBuffer)\
             ->getMaterial(PRIV(mMaterialBuffer), i);
  if (material)
  {
    buildHitRecordFromGBufferWithLight(&hitRecord, LIGHT(curLight), i);
    color = PRIV(mVPLBuffer)->getColorPtr(PRIV(mVPLBuffer), i);
    k = material->shade(material, &hitRecord, curLight);
    toAdd.r = k * (material->color.r * LIGHT(curLight)->color.r);
    toAdd.g = k * (material->color.g * LIGHT(curLight)->color.g);
    toAdd.b = k * (material->color.b * LIGHT(curLight)->color.b);
    instRadiosityMapVPLColor(self, curLight, &toAdd);
    color->r += toAdd.r;
    color->g += toAdd.g;
    color->b += toAdd.b;
    self->private.mStepBuffer->offsetSet(self->private.mStepBuffer, i, toAdd);
  }
}

static void	instRadiosityApplyVPL(InstRadiosityClass *self,
                                      PointLightClass *light)
{
  int		i;

  i = 0;
  while (i < RTEngineGetSingletonPtr()->getSceneHeight()
           * RTEngineGetSingletonPtr()->getSceneWidth())
  {
    instRadiosityAddVPLColorToBuffer(self, light, i);
    i++;
  }
}

static void		instRadiosityCreateFirstBounceVPLs(
  InstRadiosityClass *self,
  PointLightClass *light)
{
  int			iSample;
  SamplerClass		*sampler;
  RaycastClass		*tracer;
  HitRecordStruct	hitRec;
  RayStruct		ray;

  printf("pl\n");
  sampler = self->private.mFirstBounceSampler;
  tracer = new(Raycast);
  iSample = 0;
  ray.origin = light->mPos;
  while (iSample < sampler->getNumSamples(sampler))
  {
    ray.direction = sampler->nextSphereSample(sampler);
    tracer->hit(tracer, &ray, &hitRec);
    if (hitRec.hit)
      instRadiosityAddVPL(self, &hitRec, LIGHT(light), 0);
    else
      printf("no hit\n");
    iSample++;
  }
}

static void		instRadiositySetBounceDirection(InstRadiosityClass *self,
                                                        Vector3D *direction,
                                                        HitRecordStruct *hitRec)
{
  float		k;
  Vector3D	n;

  *direction = hitRec->ray.direction;
  vectOpTimes(direction, -1.);
  k = vectDotProduct(&hitRec->normal, &hitRec->ray.direction);
  n = hitRec->normal;
  vectOpTimes(&n, 2.f * k);
  vectOpPlus(direction, &n);
  vectNormalize(direction);
  vectOpTimes(direction, -1);
}

static void		instRadiosityCreateBounceFromVPL(
  InstRadiosityClass *self,
  PointLightClass *curLight,
  HitRecordStruct *lastHit)
{
  Vector3D		direction;
  RaycastClass		*tracer;
  HitRecordStruct	hitRec;
  RayStruct		ray;

  tracer = RAYCAST(TracerManagerGetTracer(eRaycast));
  instRadiositySetBounceDirection(self, &direction, lastHit);
  ray.origin = curLight->mPos;
  ray.direction = direction;
  tracer->hit(tracer, &ray, &hitRec);
  if (hitRec.hit)
    instRadiosityAddVPL(self, &hitRec, LIGHT(curLight), 1);
  else
    printf("no hhit\n");
}

static void	instRadiosityApplyLoopCreate(InstRadiosityClass *self)
{
  int		numLights;
  int		i;
  LightClass	*curLight;

  i = 0;
  numLights = self->private.mLightVect->size(self->private.mLightVect);
  while (i < numLights)
  {
    curLight = self->private.mLightVect->mPtrs[i];
    if (RTTI(curLight) == RTTI(PointLight))
      instRadiosityCreateFirstBounceVPLs(self, POINTLIGHT(curLight));
    i++;
  }
}

static void	instRadiosityApplyLoopSet(InstRadiosityClass *self)
{
  int		numLights;
  int		i;

  i = 0;
  numLights = self->private.mVPLVect->size(self->private.mVPLVect);
  printf("NUMLIGHT %d\n", numLights);
  if (self->private.mHasBounce)
  {
    while (i < numLights)
    {
      printf("bounce: light %d\n", i);
      instRadiosityCreateBounceFromVPL(self, self->private.mVPLVect->mPtrs[i],
          self->private.mHitRecVect->mPtrs[i]);
      i++;
    }
  }
}

static void	instRadiosityApplyLoop(InstRadiosityClass *self)
{
  int		i;
  LightClass	*curLight;

  instRadiosityApplyLoopCreate(self);
  instRadiosityApplyLoopSet(self);
  i = 0;
  while (i < self->private.mVPLVect->size(self->private.mVPLVect))
  {
    printf("calc %d\n", i);
    curLight = self->private.mVPLVect->mPtrs[i];
    instRadiosityApplyVPL(self, POINTLIGHT(curLight));
    instRadiosityDisplayStep(self);
    i++;
  }
  i = 0;
  while (i < self->private.mSpheresVec->size(self->private.mSpheresVec))
  {
    self->private.mScene->addObject(self->private.mScene,
      self->private.mSpheresVec->mPtrs[i]);
    i++;
  }
  instRadiosityUpdateCurrentState(self);
  printf("Done.\n");
}

static void	instRadiosityDisplayStep(InstRadiosityClass *self)
{
  int		i;
  Color		color;
  Color		*VPLColor;
  Color		*lastColor;

  i = 0;
  while (i < RTEngineGetSingletonPtr()->getSceneHeight()
           * RTEngineGetSingletonPtr()->getSceneWidth())
  {
    color = PRIV(mCurrentState)->getColor(PRIV(mCurrentState), i);
    VPLColor = PRIV(mVPLBuffer)->getColorPtr(PRIV(mVPLBuffer), i);
    lastColor = PRIV(mStepBuffer)->getColorPtr(PRIV(mStepBuffer), i);
    color.r += VPLColor->r + lastColor->r * 10;
    color.g += VPLColor->g + lastColor->g * 10;
    color.b += VPLColor->b + lastColor->b * 10;
    self->private.mDisplayedBuffer->offsetSet(self->private.mDisplayedBuffer,
                                              i, color);
    i++;
  }
  GraphicsManagerGetSingletonPtr()\
    ->display(VOID(self->private.mDisplayedBuffer));
}

static void	instRadiosityUpdateCurrentState(InstRadiosityClass *self)
{
  int		i;
  Color		*toAdd;
  Color		*color;

  i = 0;
  while (i < RTEngineGetSingletonPtr()->getSceneHeight()
           * RTEngineGetSingletonPtr()->getSceneWidth())
  {
    toAdd = PRIV(mVPLBuffer)->getColorPtr(PRIV(mVPLBuffer), i);
    color = PRIV(mCurrentState)->getColorPtr(PRIV(mCurrentState), i);
    color->r += toAdd->r;
    color->g += toAdd->g;
    color->b += toAdd->b;
    i++;
  }
}

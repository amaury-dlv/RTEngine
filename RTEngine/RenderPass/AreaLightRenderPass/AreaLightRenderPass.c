#include <string.h>

#include "AreaLightRenderPass.h"
#include "Color.h"
#include "RenderingScene.h"
#include "RTEngine.h"
#include "Maths.h"
#include "PointLight.h"
#include "TracerManager.h"
#include "Raycast.h"
#include "Sphere.h"
#include "Utils.h"
#include "Transparent.h"

static void     AreaLightRenderPassConstructor();
static void     AreaLightRenderPassDestructor();
static void	areaLightInit(AreaLightRenderPassClass *self);
static void	areaLightApplyPixel(AreaLightRenderPassClass *self,
                                    int x, int y);
static void	areaLightApplyPixelSample(AreaLightRenderPassClass *self,
                                          float x, float y, Color *col);
static void	areaLightUpdateCurrentState(AreaLightRenderPassClass *self);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static AreaLightRenderPassClass       _descr = { /* AreaLightRenderPassClass */
  { /* RenderPassClass */
    { /* Class */
      sizeof(AreaLightRenderPassClass),
      __RTTI_AREALIGHTRENDERPASS, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
      &AreaLightRenderPassConstructor,
      &AreaLightRenderPassDestructor,
    },
    &areaLightInit,
    0, /* apply */
    0, /* applyLoop */
    VOID(&areaLightApplyPixel),
    0,
    VOID(&areaLightUpdateCurrentState),
  },
  {
    0, /* mMatBuffer */
    0, /* mLightBuffer */
    0, /* mCurrentState */
    0, /* mTracer */
    0, /* mSampler */
    10 /* mSamples */
  }
};

Class *AreaLightRenderPass = (Class*) &_descr;

static void     AreaLightRenderPassConstructor(AreaLightRenderPassClass *self,
                                         va_list *list)
{
  int		width;
  int		height;

  RenderPass->__constructor__(self, list);
  width = RTEngineGetSingletonPtr()->getSceneWidth();
  height = RTEngineGetSingletonPtr()->getSceneHeight();
  self->private.mTracer = RAYCAST(TracerManagerGetTracer(eRaycast));
  self->private.mLightBuffer = new(ColorBuffer, width, height);
}

static void     AreaLightRenderPassDestructor(AreaLightRenderPassClass *self)
{
  RenderPass->__destructor__(self);
}

void		*areaLightRenderPassUnserialize(SceneSerializerClass *serializer,
                                            xmlNodePtr node)
{
  AreaLightRenderPassClass	*self;
  char				*str;

  self = new(AreaLightRenderPass);
  if (renderPassUnserializeSet(serializer, node, self) != 0)
    return (0);
  if (((str = (char *) xmlGetProp(node, XMLSTR("samples")))
       && setIntFromStr(str, &PRIV(mSamples))))
    return (0);
  return (self);
}

static void	areaLightInit(AreaLightRenderPassClass *self)
{
  GBufferClass	*gBuffer;

  gBuffer = RenderingSceneGetSingletonPtr()->getGBuffer();
  self->private.mMatBuffer =
    MATERIALBUF(gBuffer->getBuffer(gBuffer, RTTI(MaterialBuffer)));
  self->private.mCurrentState =
    RenderingSceneGetSingletonPtr()->getCurrentState();
  self->private.mSampler = SamplerManagerGetSingletonPtr()\
                           ->getSampler(eJittered,
                               self->private.mSamples,
                               83,
                               100);
}

static void		areaLightApplyPixelSample(AreaLightRenderPassClass *self,
                                                  float x, float y, Color *col)
{
  RayStruct		ray;
  HitRecordStruct	hitRecord;
  CameraClass*		camera;
  int			i;
  Point2D		sample;

  camera = RenderingSceneGetSingletonPtr()->getCurrentScene()\
           ->getCamera(RenderingSceneGetSingletonPtr()->getCurrentScene());
  i = 0;
  while (i < PRIV(mSamples))
  {
    sample = PRIV(mSampler)->nextSample(PRIV(mSampler));
    ray = camera->getRay(camera, (x + sample.x), (y + sample.y));
    hitRecordReset(&hitRecord);
    if (rayIsValid(&ray)
        && PRIV(mTracer)->hit(PRIV(mTracer), &ray, &hitRecord)
	&& hitRecord.material)
      colorAdd(col, COLORPTR(hitRecord.material\
        ->shadeAreaLight(hitRecord.material, &hitRecord)));
    ++i;
  }
}

static void	areaLightApplyPixel(AreaLightRenderPassClass *self,
                                    int x, int y)
{
  Color		*col;

  col = PRIV(mLightBuffer)->getColorPtr(PRIV(mLightBuffer),
    x + y * RTEngineGetSingletonPtr()->getSceneWidth());
  areaLightApplyPixelSample(self,
    x - 0.5 * RTEngineGetSingletonPtr()->getSceneWidth(),
    y - 0.5 * RTEngineGetSingletonPtr()->getSceneHeight(),
    col);
  colorMult(col, 1.f / PRIV(mSamples));
}

static void		areaLightUpdateCurrentState(AreaLightRenderPassClass *self)
{
  int			i;
  Color			*color;
  Color			*colorPtr;

  i = 0;
  PRIV(mCurrentState)->reset(PRIV(mCurrentState));
  while (i < RTEngineGetSingletonPtr()->getSceneHeight()
           * RTEngineGetSingletonPtr()->getSceneWidth())
  {
    color = self->private.mLightBuffer\
            ->getColorPtr(self->private.mLightBuffer, i);
    colorPtr = self->private.mCurrentState\
            ->getColorPtr(self->private.mCurrentState, i);
    colorPtr->r += color->r;
    colorPtr->g += color->g;
    colorPtr->b += color->b;
    i++;
  }
}

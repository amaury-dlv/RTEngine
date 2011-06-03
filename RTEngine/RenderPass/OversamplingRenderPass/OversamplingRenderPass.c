#include <math.h>
#include <time.h>

#include "my_omp.h"
#include "RTTI.h"
#include "Point2D.h"
#include "RTEngine.h"
#include "RenderingScene.h"
#include "GBuffer.h"
#include "OversamplingRenderPass.h"
#include "SamplerManager.h"
#include "SceneSerializer.h"
#include "Utils.h"

static void     oversamplingRenderPassConstructor();
static void     oversamplingRenderPassDestructor();
static void	oversamplingRenderPassApplyPixel(
  OversamplingRenderPassClass* self, int x, int y);
static void	oversamplingRenderPassInit(RenderPassClass *self);
static void	oversamplingRenderPassUpdateCurrentState(
  OversamplingRenderPassClass *self);
static void	oversamplingRenderPassApplyPixelSample(
  OversamplingRenderPassClass *self, float x, float y, Color *col);
static void	oversamplingRenderPassApplyLoop(
  OversamplingRenderPassClass* self);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static OversamplingRenderPassClass       _descr = { /* OversamplingRenderPassClass */
  { /* RenderPassClass */
    { /* Class */
      sizeof(OversamplingRenderPassClass),
      __RTTI_OVERSAMPLINGRENDERPASS,
      &oversamplingRenderPassConstructor,
      &oversamplingRenderPassDestructor,
    },
    &oversamplingRenderPassInit,
    0, /* apply */
    VOID(&oversamplingRenderPassApplyLoop), /* applyLoop */
    VOID(&oversamplingRenderPassApplyPixel), /* renderPassPixel (pure virtual) */
    0,
    VOID(&oversamplingRenderPassUpdateCurrentState),
  },
  { /* private */
    0, /* mBuffer */
    0, /* mTracer */
    4, /* mNumSamples */
    0 /* mSampler */
  }
};

Class* OversamplingRenderPass = (Class*) &_descr;

static void     oversamplingRenderPassConstructor(
  OversamplingRenderPassClass* self,
  va_list* list)
{
  RenderPass->__constructor__(self, list);
  RENDERPASS(self)->applyLoop = VOID(&oversamplingRenderPassApplyLoop);
  self->private.mTracer = RAYCAST(TracerManagerGetTracer(eRaycast));
}

static void     oversamplingRenderPassDestructor(RenderPassClass* self)
{
  RenderPass->__destructor__(self);
}

void				*oversamplingRenderPassUnserialize(
  SceneSerializerClass *serializer,
  xmlNodePtr node)
{
  OversamplingRenderPassClass	*self;
  char				*strNumSamples;

  self = new(OversamplingRenderPass);
  if (renderPassUnserializeSet(serializer, node, self) != 0)
    return (0);
  serializer->setStrsFromProp(serializer, node, "aa", &strNumSamples, NULL);
  if (setIntFromStr(strNumSamples, &self->private.mNumSamples) != 0)
  {
    fprintf(stderr, "Bad attributes.\n");
    exit(1);
  }
  return (self);
}

static void	oversamplingRenderPassInit(RenderPassClass *self)
{
  OversamplingRenderPassClass	*this;

  this = VOID(self);
  this->private.mBuffer = RenderingSceneGetSingletonPtr()->getCurrentState();
  this->private.mSampler = SamplerManagerGetSingletonPtr()\
                           ->getSampler(eMultiJittered,
                               this->private.mNumSamples,
                               RTEngineGetSingletonPtr()->getDefaultNumSets(),
                               100);
}

static void		oversamplingRenderPassApplyPixelSample(
  OversamplingRenderPassClass *self, float x, float y, Color *col)
{
  RayStruct		ray;
  HitRecordStruct	hitRecord;
  CameraClass*		camera;
  int			i;
  Point2D		sample;

  camera = RenderingSceneGetSingletonPtr()->getCurrentScene()\
           ->getCamera(RenderingSceneGetSingletonPtr()->getCurrentScene());
  i = 0;
  while (i < PRIV(mNumSamples))
  {
    sample = PRIV(mSampler)->nextSample(PRIV(mSampler));
    ray = camera->getRay(camera, (x + sample.x), (y + sample.y));
    hitRecordReset(&hitRecord);
    if (rayIsValid(&ray)
        && PRIV(mTracer)->hit(PRIV(mTracer), &ray, &hitRecord)
	&& hitRecord.material)
      colorAdd(col, COLORPTR(hitRecord.material->shadeRGB(hitRecord.material,
                                                          &hitRecord)));
    ++i;
  }
}

static void	oversamplingRenderPassApplyPixel(
  OversamplingRenderPassClass* self, int x, int y)
{
  Color		*col;

  col = PRIV(mBuffer)->getColorPtr(PRIV(mBuffer),
    x + y * RTEngineGetSingletonPtr()->getSceneWidth());
  oversamplingRenderPassApplyPixelSample(self,
    x - 0.5 * RTEngineGetSingletonPtr()->getSceneWidth(),
    y - 0.5 * RTEngineGetSingletonPtr()->getSceneHeight(),
    col);
  colorMult(col, 1.f / PRIV(mNumSamples));
}

static void		oversamplingRenderPassApplyLoop(
  OversamplingRenderPassClass* self)
{
  static time_t		lastdisp = 0;
  int			width;
  int			i;
  int			size;

  width = RTEngineGetSingletonPtr()->getSceneWidth();
  i = -1;
  size = width * RTEngineGetSingletonPtr()->getSceneHeight();
#ifndef USE_THREADS
  while (++i < size)
#else
# pragma omp parallel for private(i) schedule(dynamic, 40)
  for (i = 0; i < size; i++)
#endif
  {
    oversamplingRenderPassApplyPixel(self, i % width, i / width);
    if (difftime(time(NULL), lastdisp) >= 1 && OMP_NUM() == 0)
    {
      GraphicsManagerGetSingletonPtr()\
        ->display(VOID(RenderingSceneGetSingletonPtr()->getCurrentState()));
      lastdisp = time(NULL);
      RENDERPASS(self)->print(RENDERPASS(self), i, size);
    }
  }
  RENDERPASS(self)->print(RENDERPASS(self), size, size);
}

static void	oversamplingRenderPassUpdateCurrentState(
  OversamplingRenderPassClass *self)
{
}

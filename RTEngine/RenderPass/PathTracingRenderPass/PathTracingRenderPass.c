#include <string.h>
#include <time.h>

#include "my_omp.h"
#include "PathTracingRenderPass.h"
#include "Color.h"
#include "RenderingScene.h"
#include "RTEngine.h"
#include "Maths.h"
#include "PointLight.h"
#include "TracerManager.h"
#include "Sphere.h"
#include "Utils.h"

static void     PathTracingRenderPassConstructor();
static void     PathTracingRenderPassDestructor();
static void	pathTracingInit(PathTracingRenderPassClass *self);
static void	pathTracingApplyLoop(PathTracingRenderPassClass *self);
static void	pathTracingUpdateCurrentState(PathTracingRenderPassClass *);
static void	pathTracingApplyPixel(PathTracingRenderPassClass *self,
                                      int x, int y);
static void	pathTracingApplyPixelSample(PathTracingRenderPassClass *self,
                                            float x, float y, Color *col);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static PathTracingRenderPassClass       _descr = { /* PathTracingRenderPassClass */
  { /* RenderPassClass */
    { /* Class */
      sizeof(PathTracingRenderPassClass),
      __RTTI_PATHTRACINGRENDERPASS, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
      &PathTracingRenderPassConstructor,
      &PathTracingRenderPassDestructor,
    },
    &pathTracingInit,
    0, /* apply */
    VOID(&pathTracingApplyLoop), /* applyLoop */
    VOID(&pathTracingApplyPixel),
    0,
    VOID(&pathTracingUpdateCurrentState),
  },
  {
    0, /* mMatBuffer */
    0, /* mCurrentState */
    4, /* mNumSamples */
    0, /* mSampler */
    0 /* mTracer */
  }
};

Class *PathTracingRenderPass = (Class*) &_descr;

static void     PathTracingRenderPassConstructor(
  PathTracingRenderPassClass *self,
  va_list *list)
{
  RenderPass->__constructor__(self, list);
  self->private.mTracer = RAYCAST(TracerManagerGetTracer(eRaycast));
  RENDERPASS(self)->applyLoop = VOID(&pathTracingApplyLoop);
}

static void     PathTracingRenderPassDestructor(
  PathTracingRenderPassClass *self)
{
  RenderPass->__destructor__(self);
}

void		*pathTracingRenderPassUnserialize(
  SceneSerializerClass *serializer,
  xmlNodePtr node)
{
  PathTracingRenderPassClass	*self;
  char				*strNumSamples;

  self = new(PathTracingRenderPass);
  if (renderPassUnserializeSet(serializer, node, self) != 0)
    return (0);
  serializer->setStrsFromProp(serializer, node, "samples", &strNumSamples,
    NULL);
  if (setIntFromStr(strNumSamples, &self->private.mNumSamples) != 0)
  {
    fprintf(stderr, "Bad attributes.\n");
    exit(1);
  }
  return (self);
}

static void	pathTracingInit(PathTracingRenderPassClass *self)
{
  GBufferClass	*gBuffer;

  gBuffer = RenderingSceneGetSingletonPtr()->getGBuffer();
  self->private.mMatBuffer =
    MATERIALBUF(gBuffer->getBuffer(gBuffer, RTTI(MaterialBuffer)));
  self->private.mCurrentState =
    RenderingSceneGetSingletonPtr()->getCurrentState();
  self->private.mSampler = SamplerManagerGetSingletonPtr()\
                           ->getSampler(eMultiJittered,
                               self->private.mNumSamples,
                               RTEngineGetSingletonPtr()->getDefaultNumSets(),
                               (float)100.);
  PRIV(mCurrentState)->reset(PRIV(mCurrentState));
}

static void		pathTracingApplyPixelSample(
  PathTracingRenderPassClass *self,
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
  while (i < PRIV(mNumSamples))
  {
    sample = PRIV(mSampler)->nextSample(PRIV(mSampler));
    ray = camera->getRay(camera, (x + sample.x), (y + sample.y));
    hitRecordReset(&hitRecord);
    if (rayIsValid(&ray)
        && PRIV(mTracer)->hit(PRIV(mTracer), &ray, &hitRecord)
	&& hitRecord.material)
      colorAdd(col, COLORPTR(hitRecord.material->shadePath(hitRecord.material,
                                                           &hitRecord)));
    ++i;
  }
}

static void	pathTracingApplyPixel(PathTracingRenderPassClass* self,
                                      int x, int y)
{
  Color		*col;

  col = PRIV(mCurrentState)->getColorPtr(PRIV(mCurrentState),
    x + y * RTEngineGetSingletonPtr()->getSceneWidth());
  pathTracingApplyPixelSample(self,
    x - 0.5 * RTEngineGetSingletonPtr()->getSceneWidth(),
    y - 0.5 * RTEngineGetSingletonPtr()->getSceneHeight(),
    col);
  colorMult(col, 1.f / PRIV(mNumSamples));
}

static void		pathTracingApplyLoop(PathTracingRenderPassClass* self)
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
# pragma omp parallel for private(i) schedule(dynamic)
  for (i = 0; i < size; i++)
#endif
  {
    pathTracingApplyPixel(self, i % width, i / width);
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

static void	pathTracingUpdateCurrentState(PathTracingRenderPassClass *self)
{
}

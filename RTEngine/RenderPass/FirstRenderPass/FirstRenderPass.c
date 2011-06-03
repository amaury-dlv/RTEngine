#include "RTTI.h"
#include "Point2D.h"
#include "RTEngine.h"
#include "RenderingScene.h"
#include "GBuffer.h"
#include "FirstRenderPass.h"
#include "SamplerManager.h"
#include "FishEye.h"

static void     firstRenderPassConstructor();
static void     firstRenderPassDestructor();
static void	firstRenderPassPixel(RenderPassClass* self, int x, int y);
static void	firstRenderPassInit(FirstRenderPassClass *self);
static void	firstRenderPassUpdateCurrentState(RenderPassClass *self);
static void	fillBuffersFromHitRecord(FirstRenderPassClass *, int x, int y,
                                         HitRecordStruct *);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static FirstRenderPassClass       _descr = { /* FirstRenderPassClass */
  { /* RenderPassClass */
    { /* Class */
      sizeof(FirstRenderPassClass),
      0, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
      &firstRenderPassConstructor,
      &firstRenderPassDestructor,
    },
    &firstRenderPassInit,
    0, /* apply */
    0, /* applyLoop */
    VOID(&firstRenderPassPixel), /* renderPassPixel (pure virtual) */
    0,
    VOID(&firstRenderPassUpdateCurrentState),
  },
  { /* private */
    0, /* mTracer */
    {0, 0, 0}, /* mBGColor */
    /* buffer */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
  }
};

Class* FirstRenderPass = (Class*) &_descr;

static void     firstRenderPassConstructor(FirstRenderPassClass* self,
                                             va_list* list)
{
  GBufferClass	*gBuffer;
  void		*localBuffer;
  int		width;
  int		height;

  RenderPass->__constructor__(self, list);
  width = RTEngineGetSingletonPtr()->getSceneWidth();
  height = RTEngineGetSingletonPtr()->getSceneHeight();
  gBuffer = RenderingSceneGetSingletonPtr()->getGBuffer();
  gBuffer->addBuffer(gBuffer, new(ColorBuffer, width, height));
  gBuffer->addBuffer(gBuffer, new(DepthBuffer, width, height));
  gBuffer->addBuffer(gBuffer, new(PointBuffer, width, height));
  localBuffer = new(PointBuffer, width, height);
  RTTI(localBuffer) = __RTTI_LOCALPOINTBUFFER;
  gBuffer->addBuffer(gBuffer, IMAGEBUF(localBuffer));
  gBuffer->addBuffer(gBuffer, new(NormalBuffer, width, height));
  gBuffer->addBuffer(gBuffer, new(RayBuffer, width, height));
  gBuffer->addBuffer(gBuffer, new(MaterialBuffer, width, height));
  gBuffer->addBuffer(gBuffer, new(BoolBuffer, width, height));
  self->private.mTracer = new(Raycast);
}

static void     firstRenderPassDestructor(FirstRenderPassClass* self)
{
  RenderPass->__destructor__(self);
}

static void	firstRenderPassInit(FirstRenderPassClass *self)
{
  GBufferClass*		GBuffer;

  GBuffer = RenderingSceneGetSingletonPtr()->getGBuffer();
  PRIV(rayBuffer) =
    VOID(GBuffer->getBuffer(GBuffer, __RTTI_RAYBUFFER));
  PRIV(colorBuffer) =
    VOID(GBuffer->getBuffer(GBuffer, __RTTI_COLORBUFFER));
  PRIV(normalBuffer) =
    VOID(GBuffer->getBuffer(GBuffer, __RTTI_NORMALBUFFER));
  PRIV(pointBuffer) =
    VOID(GBuffer->getBuffer(GBuffer, __RTTI_POINTBUFFER));
  PRIV(localPointBuffer) =
    VOID(GBuffer->getBuffer(GBuffer, __RTTI_LOCALPOINTBUFFER));
  PRIV(depthBuffer) =
    VOID(GBuffer->getBuffer(GBuffer, __RTTI_DEPTHBUFFER));
  PRIV(materialBuffer) =
    VOID(GBuffer->getBuffer(GBuffer, __RTTI_MATBUFFER));
  PRIV(insideBuffer) =
    VOID(GBuffer->getBuffer(GBuffer, __RTTI_BOOLBUFFER));
  PRIV(mBGColor) =
    RenderingSceneGetSingletonPtr()->getCurrentScene()->mBGColor;
}

/* vvv */

static void		fillBuffersFromHitRecord(FirstRenderPassClass* self,
                                                 int x, int y,
                                                 HitRecordStruct* rec)
{
  PRIV(rayBuffer)->setFromStruct(PRIV(rayBuffer), x, y, &rec->ray);
  PRIV(normalBuffer)->set(PRIV(normalBuffer), x, y, rec->normal);
  PRIV(pointBuffer)->set(PRIV(pointBuffer), x, y, rec->hitPoint);
  PRIV(localPointBuffer)->set(PRIV(localPointBuffer), x, y,
    rec->localHitPoint);
  PRIV(depthBuffer)->setFromHitRecord(PRIV(depthBuffer), x, y, rec);
  PRIV(insideBuffer)->set(PRIV(insideBuffer), x, y, rec->inside);
  if (rec->material)
  {
    PRIV(colorBuffer)->set(PRIV(colorBuffer), x, y, rec->material->color);
    PRIV(materialBuffer)->set(PRIV(materialBuffer), x, y, rec->material);
  }
  else
    PRIV(colorBuffer)->set(PRIV(colorBuffer), x, y, PRIV(mBGColor));
}

static void	firstRenderPassPixel(RenderPassClass* self, int x, int y)
{
  RayStruct		ray;
  HitRecordStruct	hitRecord;
  CameraClass*		camera;
  RaycastClass*		tracer;
  int			width;
  int			height;

  hitRecordReset(&hitRecord);
  width = RTEngineGetSingletonPtr()->getSceneWidth();
  height = RTEngineGetSingletonPtr()->getSceneHeight();
  tracer = FIRSTRPASS(self)->private.mTracer;
  camera = RenderingSceneGetSingletonPtr()->getCurrentScene()\
           ->getCamera(RenderingSceneGetSingletonPtr()->getCurrentScene());
  ray = camera->getRay(camera, (x - width * 0.5 + 0.5),
                               (y - height * 0.5 + 0.5));
  if (rayIsValid(&ray))
    tracer->hit(tracer, &ray, &hitRecord);
  fillBuffersFromHitRecord(FIRSTRPASS(self), x, y, &hitRecord);
}

/* ^^^ */

static void		firstRenderPassUpdateCurrentState(
  RenderPassClass *self)
{
  int			i;
  ColorBufferClass	*currentState;
  int			size;
  GBufferClass		*gBuffer;
  ColorBufferClass	*colorBuffer;

  i = 0;
  size = RTEngineGetSingletonPtr()->getSceneWidth()
         * RTEngineGetSingletonPtr()->getSceneHeight();
  gBuffer = RenderingSceneGetSingletonPtr()->getGBuffer();
  currentState = RenderingSceneGetSingletonPtr()->getCurrentState();
  colorBuffer = COLORBUF(gBuffer->getBuffer(gBuffer, __RTTI_COLORBUFFER));
  while (i < size)
  {
    currentState->offsetSet(currentState, i,
      colorBuffer->getColor(colorBuffer, i));
    i++;
  }
}

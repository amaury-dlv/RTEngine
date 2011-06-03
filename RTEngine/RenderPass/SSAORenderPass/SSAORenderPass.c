#include "SSAORenderPass.h"
#include "RTEngine.h"
#include "RenderingScene.h"
#include "GBuffer.h"
#include "PointBuffer.h"
#include "NormalBuffer.h"
#include "MaterialBuffer.h"
#include "LightBuffer.h"
#include "Scene.h"
#include "Vector3D.h"
#include "Light.h"
#include "RTTI.h"
#include "Utils.h"
#include "Maths.h"
#include "ColorBuffer.h"
#include "HitRecord.h"
#include "SceneSerializer.h"

static void     SSAORenderPassConstructor();
static void     SSAORenderPassDestructor();
static void	SSAORenderPassPixel(SSAORenderPassClass* self, int x, int y);
static void	SSAORenderPassInit(SSAORenderPassClass *self);
static void	SSAOUpdateCurrentState(SSAORenderPassClass *self);
static float	SSAORenderPassPixelSample(SSAORenderPassClass *self,
                                          Vector3D ep, Vector3D fres,
					  float radd);
static int	SSAORenderPassPixelInit(SSAORenderPassClass *self,
                                        int x, int y, Vector3D *fres);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static SSAORenderPassClass       _descr = { /* SSAORenderPassClass */
  { /* RenderPassClass */
    { /* Class */
      sizeof(SSAORenderPassClass),
      __RTTI_SSAORENDERPASS, /* RTTI */
      &SSAORenderPassConstructor,
      &SSAORenderPassDestructor,
    },
    &SSAORenderPassInit,
    0, /* apply */
    0, /* applyLoop */
    VOID(&SSAORenderPassPixel), /* renderPassPixel (pure virtual) */
    0,
    VOID(&SSAOUpdateCurrentState),
  },
  {
    0.38, /* totStrength */
    2., /* strength */
    .0002, /* falloff */
    1., /* rad */
    16, /* samples */
    6, /* blurKernelSize */
    0, /* sphereSampler */
    0, /* normalSampler */
    0, /* light */
    0, /* lightBuf */
    0, /* pointBuf */
    0, /* normalBuf */
    0, /* depthBuf */
    {0, 0}, /* coord */
    {0, 0, 0}, /* point */
    {0, 0, 0}, /* normal */
    0 /* depth */
  }
};

Class* SSAORenderPass = (Class*) &_descr;

static void     SSAORenderPassConstructor(SSAORenderPassClass* self,
                                             va_list* list)
{
  RenderPass->__constructor__(self, list);
  PRIV(sphereSampler) =
    SamplerManagerGetSingletonPtr()->getSampler(eMultiJittered, PRIV(samples),
                                                83, (float)100.);
  PRIV(sphereSampler)->mapSamplesToSphere(PRIV(sphereSampler));
  PRIV(normalSampler) =
    SamplerManagerGetSingletonPtr()->getSampler(eMultiJittered, 64,
      83, (float)100.);
  PRIV(normalSampler)->mapSamplesToSphere(PRIV(normalSampler));
}

static void     SSAORenderPassDestructor()
{
}

void		*SSAORenderPassUnserialize(SceneSerializerClass *serializer,
                                           xmlNodePtr node)
{
  SSAORenderPassClass	*self;
  char			*str;

  self = new(SSAORenderPass);
  if (renderPassUnserializeSet(serializer, node, self) != 0)
    return (0);
  if (((str = (char *) xmlGetProp(node, XMLSTR("samples")))
       && setIntFromStr(str, &PRIV(samples)))
      || ((str = (char *) xmlGetProp(node, XMLSTR("totStrength")))
          && setFloatFromStr(str, &PRIV(totStrength)))
      || ((str = (char *) xmlGetProp(node, XMLSTR("strength")))
          && setFloatFromStr(str, &PRIV(strength)))
      || ((str = (char *) xmlGetProp(node, XMLSTR("falloff")))
          && setFloatFromStr(str, &PRIV(falloff)))
      || ((str = (char *) xmlGetProp(node, XMLSTR("blurKernelSize")))
          && setIntFromStr(str, &PRIV(blurKernelSize)))
      || ((str = (char *) xmlGetProp(node, XMLSTR("rad")))
          && setFloatFromStr(str, &PRIV(rad))))
    return (0);
  return (self);
}

static void	SSAORenderPassInit(SSAORenderPassClass *self)
{
  SceneClass	*scene;
  GBufferClass	*gBuffer;

  scene = RenderingSceneGetSingletonPtr()->getCurrentScene();
  gBuffer = RenderingSceneGetSingletonPtr()->getGBuffer();
  PRIV(light) = LIGHT(scene->getAmbientPtr(scene));
  PRIV(lightBuf) = gBuffer->getLightBuffer(gBuffer, PRIV(light)->id);
  PRIV(pointBuf) =
    POINTBUFFER(gBuffer->getBuffer(gBuffer, __RTTI_POINTBUFFER));
  PRIV(normalBuf) =
    NORMALBUFFER(gBuffer->getBuffer(gBuffer, __RTTI_NORMALBUFFER));
  PRIV(depthBuf) =
    DEPTHBUFFER(gBuffer->getBuffer(gBuffer, __RTTI_DEPTHBUFFER));
}

static float	SSAORenderPassPixelSample(SSAORenderPassClass *self,
                                          Vector3D ep, Vector3D fres,
					  float radd)
{
  Vector3D	occNorm;
  float		occDepth;
  int		xy;
  Vector3D	ray;
  float		zdiff;
  float		ndiff;

  ray = PRIV(sphereSampler)->nextSphereSample(PRIV(sphereSampler));
  ray = vectReflect(&ray, &fres);
  vectOpTimes(&ray, radd);
  if (vectDotProduct(&ray, &PRIV(normal)) < .0)
    vectOpPlus(&ep, &ray);
  xy = ep.x + ep.y * RTEngineGetSingletonPtr()->getSceneWidth();
  occNorm = PRIV(normalBuf)->getNormal(PRIV(normalBuf), xy);
  occDepth = PRIV(depthBuf)->getDepth(PRIV(depthBuf), xy);
  zdiff = PRIV(depth) - occDepth;
  ndiff = (1.0 - vectDotProduct(&occNorm, &PRIV(normal)));
  return (step(PRIV(falloff), zdiff) * ndiff
          * (1.0 - smoothstep(PRIV(falloff), PRIV(strength), zdiff)));
}

static int	SSAORenderPassPixelInit(SSAORenderPassClass *self,
                                        int x, int y, Vector3D *fres)
{
  int		width;
  int		xy;

  width = RTEngineGetSingletonPtr()->getSceneWidth();
  xy = x + y * width;
  *fres = PRIV(normalSampler)->nextSphereSample(PRIV(normalSampler));
  vectOpTimes(fres, 2.0);
  vectOpMinus(fres, &(Vector3D){1., 1., 1.});
  vectNormalize(fres);
  PRIV(point) = PRIV(pointBuf)->getPoint(PRIV(pointBuf), xy);
  PRIV(normal) = PRIV(normalBuf)->getNormal(PRIV(normalBuf), xy);
  PRIV(depth) = PRIV(depthBuf)->getDepth(PRIV(depthBuf), xy);
  return (xy);
}

static void	SSAORenderPassPixel(SSAORenderPassClass* self,
                                     int x, int y)
{
  int		xy;
  float		ao;
  Vector3D	fres;
  int		i;
  float		radd;

  xy = SSAORenderPassPixelInit(self, x, y, &fres);
  radd = PRIV(rad) / PRIV(depth);
  ao = 0;
  i = 0;
  while (i < PRIV(samples))
  {
     ao += SSAORenderPassPixelSample(self, (Vector3D){x, y, PRIV(depth)},
                                     fres, radd);
     ++i;
  }
  ao = 1.0 - PRIV(totStrength) * ao * (1. / PRIV(samples));
  ao = clamp(ao, 0, 1);
  PRIV(lightBuf)->offsetSet(PRIV(lightBuf), xy, ao);
}

static void		SSAOUpdateCurrentState(SSAORenderPassClass *self)
{
  ColorBufferClass	*currentState;
  int			size;
  GBufferClass		*gBuffer;
  ColorBufferClass	*colorBuf;
  Color			*curColor;
  Color			tmp;
  float			ratio;

  size = RTEngineGetSingletonPtr()->getSceneWidth()
         * RTEngineGetSingletonPtr()->getSceneHeight();
  gBuffer = RenderingSceneGetSingletonPtr()->getGBuffer();
  currentState = RenderingSceneGetSingletonPtr()->getCurrentState();
  PRIV(lightBuf)->blur(PRIV(lightBuf), PRIV(blurKernelSize));
  colorBuf = COLORBUF(gBuffer->getBuffer(gBuffer, __RTTI_COLORBUFFER));
  while (size--)
  {
    curColor = currentState->getColorPtr(currentState, size);
    tmp = colorBuf->getColor(colorBuf, size);
    if ((ratio = PRIV(lightBuf)->getLight(PRIV(lightBuf), size)))
    {
      curColor->r = (0.5 + 0.5 * ratio) * curColor->r;
      curColor->g = (0.5 + 0.5 * ratio) * curColor->g;
      curColor->b = (0.5 + 0.5 * ratio) * curColor->b;
    }
  }
}

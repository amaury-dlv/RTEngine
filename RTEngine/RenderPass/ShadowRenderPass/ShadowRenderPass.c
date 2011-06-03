#include "ShadowRenderPass.h"
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
#include "Maths.h"
#include "ColorBuffer.h"
#include "HitRecord.h"
#include "SceneSerializer.h"
#include "Utils.h"

static void     shadowRenderPassConstructor();
static void     shadowRenderPassDestructor();
static void	shadowRenderPassPixel(ShadowRenderPassClass* self,
  int x, int y);
static void	shadowRenderPassInit(ShadowRenderPassClass *self,
  LightClass *curLight);
static void	shadowRenderPassApply(ShadowRenderPassClass *self);
static void	shadowUpdateCurrentState(ShadowRenderPassClass *self);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static ShadowRenderPassClass       _descr = { /* ShadowRenderPassClass */
  { /* RenderPassClass */
    { /* Class */
      sizeof(ShadowRenderPassClass),
      __RTTI_SHADOWRENDERPASS, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
      &shadowRenderPassConstructor,
      &shadowRenderPassDestructor,
    },
    &shadowRenderPassInit,
    VOID(&shadowRenderPassApply), /* apply */
    0, /* applyLoop */
    VOID(&shadowRenderPassPixel), /* renderPassPixel (pure virtual) */
    0,
    VOID(&shadowUpdateCurrentState),
  },
  {
    0, /* curLight */
    0, /* curLightBuf */
    0, /* pointBuf */
    0, /* passCount */
    0 /* soft */
  }
};

Class* ShadowRenderPass = (Class*) &_descr;

double	g_last_d;
double	g_last_t;

static void     shadowRenderPassConstructor(ShadowRenderPassClass* self,
                                             va_list* list)
{
  RenderPass->__constructor__(self, list);
  RENDERPASS(self)->apply = VOID(&shadowRenderPassApply);
}

static void     shadowRenderPassDestructor()
{
}

void		*shadowRenderPassUnserialize(SceneSerializerClass *serializer,
                                            xmlNodePtr node)
{
  ShadowRenderPassClass	*self;
  char			*str;

  self = new(ShadowRenderPass);
  if (renderPassUnserializeSet(serializer, node, self) != 0)
    return (0);
  if ((str = (char *) xmlGetProp(node, XMLSTR("soft")))
       && setIntFromStr(str, &PRIV(soft)))
    return (0);
  return (self);
}

static void	shadowRenderPassInit(ShadowRenderPassClass *self,
                                     LightClass *curLight)
{
  GBufferClass	*gBuffer;

  gBuffer = RenderingSceneGetSingletonPtr()->getGBuffer();
  PRIV(curLight) = curLight;
  PRIV(curLightBuf) = gBuffer->getLightBuffer(gBuffer, curLight->id);
  PRIV(pointBuf) = POINTBUFFER(gBuffer->getBuffer(gBuffer,
    __RTTI_POINTBUFFER));
} 
static void		shadowRenderPassPixel(ShadowRenderPassClass* self,
                                             int x, int y)
{
  int			xy;
  RayStruct		shadowRay;
  float			ratio;
  float			ratio2;
  HitRecordStruct	dummy;

  hitRecordReset(&dummy);
  xy = x + y * RTEngineGetSingletonPtr()->getSceneWidth();
  shadowRay.origin = PRIV(pointBuf)->getPoint(PRIV(pointBuf), xy);
  dummy.hitPoint = shadowRay.origin;
  shadowRay.direction
    = PRIV(curLight)->getDirection(PRIV(curLight), &dummy);
  if ((ratio = PRIV(curLightBuf)->getLight(PRIV(curLightBuf), xy))
      && PRIV(curLight)->inShadow(PRIV(curLight), &shadowRay))
  {
    if (PRIV(soft) && RTTI(PRIV(curLight)) == __RTTI_POINTLIGHT)
    {
      ratio2 = ratio;
      ratio = g_last_d / (g_last_t * g_last_t);
      ratio = ratio / (ratio + 1.);
      ratio = ratio2 * ratio;
    }
    PRIV(curLightBuf)->offsetSet(PRIV(curLightBuf), xy, -ratio);
  }
}

static void		shadowUpdateCurrentState(ShadowRenderPassClass *self)
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
  colorBuf = COLORBUF(gBuffer->getBuffer(gBuffer, __RTTI_COLORBUFFER));
  while (size--)
  {
    curColor = currentState->getColorPtr(currentState, size);
    tmp = colorBuf->getColor(colorBuf, size);
    if ((ratio = PRIV(curLightBuf)->getLight(PRIV(curLightBuf), size)) < .0)
    {
      curColor->r += ratio * (tmp.r * PRIV(curLight)->color.r);
      curColor->g += ratio * (tmp.g * PRIV(curLight)->color.g);
      curColor->b += ratio * (tmp.b * PRIV(curLight)->color.b);
      PRIV(curLightBuf)->offsetSet(PRIV(curLightBuf), size, .0);
    }
  }
}

static void	shadowRenderPassApply(ShadowRenderPassClass *self)
{
  VectorClass*  lightVector;
  int		nLights;

  lightVector = RenderingSceneGetSingletonPtr()->getCurrentScene()->mLights;
  nLights = lightVector->size(lightVector);
  PRIV(passCount) = 0;
  while (PRIV(passCount) < nLights)
  {
    RENDERPASS(self)->init(self, lightVector->mPtrs[PRIV(passCount)]);
    RENDERPASS(self)->applyLoop(RENDERPASS(self));
    PRIV(passCount)++;
  }
}

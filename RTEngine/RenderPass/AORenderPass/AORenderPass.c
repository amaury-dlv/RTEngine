#include "AORenderPass.h"
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

static void     AORenderPassConstructor(AORenderPassClass *, va_list *);
static void     AORenderPassDestructor();
static void	AORenderPassPixel(AORenderPassClass *self, int x, int y);
static void	AORenderPassInit(AORenderPassClass *self, LightClass *curLight);
static void	AORenderPassApply(AORenderPassClass *self);
static void	AOUpdateCurrentState(AORenderPassClass *self);
static void	AORevertCurrentState(AORenderPassClass *self);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static AORenderPassClass       _descr = { /* AORenderPassClass */
  { /* RenderPassClass */
    { /* Class */
      sizeof(AORenderPassClass),
      __RTTI_AORENDERPASS, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
      &AORenderPassConstructor,
      &AORenderPassDestructor,
    },
    &AORenderPassInit,
    VOID(&AORenderPassApply), /* apply */
    0, /* applyLoop */
    VOID(&AORenderPassPixel), /* renderPassPixel (pure virtual) */
    0,
    VOID(&AOUpdateCurrentState),
  },
  {
    0, /* curLight */
    0, /* curLightBuf */
    0, /* materialBuf */
    0, /* mHasBlur */
    25, /* mSamples */
    0.1 /* mOcclusionFactor */
  }
};

Class* AORenderPass = (Class*) &_descr;

static void     AORenderPassConstructor(AORenderPassClass* self,
                                             va_list* list)
{
  RenderPass->__constructor__(self, list);
  RENDERPASS(self)->apply = VOID(&AORenderPassApply);
}

static void     AORenderPassDestructor()
{
}

void			*AORenderPassUnserialize(SceneSerializerClass *serializer,
                                            xmlNodePtr node)
{
  AORenderPassClass	*self;
  char			*str;

  self = new(AORenderPass);
  if (renderPassUnserializeSet(serializer, node, self) != 0)
    return (0);
  if (((str = (char *) xmlGetProp(node, XMLSTR("samples")))
       && setIntFromStr(str, &PRIV(mSamples)))
      || ((str = (char *) xmlGetProp(node, XMLSTR("occlusion")))
          && setFloatFromStr(str, &PRIV(mOcclusionFactor))))
    return (0);
  return (self);
}

static void	AORenderPassInit(AORenderPassClass *self,
                                    LightClass *curLight)
{
  GBufferClass	*gBuffer;

  gBuffer = RenderingSceneGetSingletonPtr()->getGBuffer();
  PRIV(curLight) = curLight;
  PRIV(curLightBuf) = gBuffer->getLightBuffer(gBuffer, curLight->id);
  PRIV(materialBuf) =
    MATERIALBUF(gBuffer->getBuffer(gBuffer, __RTTI_MATBUFFER));
  AMBIENT(curLight)->setOccluder(AMBIENT(curLight), PRIV(mSamples),
    PRIV(mOcclusionFactor));
}

static void		AORenderPassPixel(AORenderPassClass* self,
                                          int x, int y)
{
  MaterialClass		*material;
  int			xy;
  HitRecordStruct	hitRecord;
  float			ratio;

  xy = x + y * RTEngineGetSingletonPtr()->getSceneWidth();
  ratio = 0;
  material = PRIV(materialBuf)\
             ->getMaterial(PRIV(materialBuf), xy);
  if (material)
  {
    buildHitRecordFromGBufferWithLight(&hitRecord, PRIV(curLight), xy);
    ratio = PRIV(curLight)->l(PRIV(curLight), &hitRecord).r;
  }
  PRIV(curLightBuf)->offsetSet(PRIV(curLightBuf), xy, ratio);
}

static void		AORevertCurrentState(AORenderPassClass *self)
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
    ratio = PRIV(curLightBuf)->getLight(PRIV(curLightBuf), size);
    tmp = colorBuf->getColor(colorBuf, size);
    curColor->r -= ratio * (tmp.r * LIGHT(PRIV(curLight))->color.r);
    curColor->g -= ratio * (tmp.g * LIGHT(PRIV(curLight))->color.g);
    curColor->b -= ratio * (tmp.b * LIGHT(PRIV(curLight))->color.b);
  }
}

static void		AOUpdateCurrentState(AORenderPassClass *self)
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
    ratio = PRIV(curLightBuf)->getLight(PRIV(curLightBuf), size);
    tmp = colorBuf->getColor(colorBuf, size);
    curColor->r += ratio * (tmp.r * LIGHT(PRIV(curLight))->color.r);
    curColor->g += ratio * (tmp.g * LIGHT(PRIV(curLight))->color.g);
    curColor->b += ratio * (tmp.b * LIGHT(PRIV(curLight))->color.b);
  }
}

static void	AORenderPassApply(AORenderPassClass *self)
{
  AmbientClass	*ambientLight;

  ambientLight = RenderingSceneGetSingletonPtr()->getCurrentScene()->mAmbient;
  RENDERPASS(self)->init(self, ambientLight);
  AORevertCurrentState(self);
  RENDERPASS(self)->applyLoop(RENDERPASS(self));
  if (PRIV(mHasBlur))
    PRIV(curLightBuf)->blur(PRIV(curLightBuf), 16);
}

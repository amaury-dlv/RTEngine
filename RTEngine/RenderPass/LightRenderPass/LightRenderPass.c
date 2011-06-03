#include "LightRenderPass.h"
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

static void     lightRenderPassConstructor();
static void     lightRenderPassDestructor();
static void	lightRenderPassPixel(LightRenderPassClass *self, int x, int y);
static void	lightRenderPassInit(LightRenderPassClass *self,
  LightClass *curLight);
static void	lightRenderPassApply(LightRenderPassClass *self);
static void	lightUpdateCurrentState(LightRenderPassClass *self);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static LightRenderPassClass       _descr = { /* LightRenderPassClass */
  { /* RenderPassClass */
    { /* Class */
      sizeof(LightRenderPassClass),
      __RTTI_LIGHTRENDERPASS, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
      &lightRenderPassConstructor,
      &lightRenderPassDestructor,
    },
    &lightRenderPassInit,
    VOID(&lightRenderPassApply), /* apply */
    0, /* applyLoop */
    VOID(&lightRenderPassPixel), /* renderPassPixel (pure virtual) */
    0,
    VOID(&lightUpdateCurrentState),
  },
  {
    0, /* curLight */
    0, /* curLightBuf */
    0, /* materialBuf */
    0 /* passCount */
  }
};

Class* LightRenderPass = (Class*) &_descr;

static void     lightRenderPassConstructor(LightRenderPassClass* self,
                                             va_list* list)
{
  RenderPass->__constructor__(self, list);
  RENDERPASS(self)->apply = VOID(&lightRenderPassApply);
}

static void     lightRenderPassDestructor()
{
}

void		*lightRenderPassUnserialize(SceneSerializerClass *serializer,
                                            xmlNodePtr node)
{
  LightRenderPassClass	*self;

  self = new(LightRenderPass);
  if (renderPassUnserializeSet(serializer, node, self) != 0)
    return (0);
  return (self);
}

static void	lightRenderPassInit(LightRenderPassClass *self,
                                    LightClass *curLight)
{
  GBufferClass	*gBuffer;

  gBuffer = RenderingSceneGetSingletonPtr()->getGBuffer();
  PRIV(curLight) = curLight;
  PRIV(curLightBuf) = new(LightBuffer,
	RTEngineGetSingletonPtr()->getSceneWidth(),
	RTEngineGetSingletonPtr()->getSceneHeight(), curLight->id);
  gBuffer->addBuffer(gBuffer, IMAGEBUF(PRIV(curLightBuf)));
  PRIV(materialBuf) =
    MATERIALBUF(gBuffer->getBuffer(gBuffer, __RTTI_MATBUFFER));
}

static void		lightRenderPassPixel(LightRenderPassClass* self,
                                             int x, int y)
{
  MaterialClass		*material;
  int			xy;
  HitRecordStruct	hitRecord;
  float			ratio;

  xy = x + y * RTEngineGetSingletonPtr()->getSceneWidth();
  material = PRIV(materialBuf)\
             ->getMaterial(PRIV(materialBuf), xy);
  ratio = 0;
  if (material)
  {
    buildHitRecordFromGBufferWithLight(&hitRecord, PRIV(curLight), xy);
    if (PRIV(passCount) == -1)
      ratio = PRIV(curLight)->l(PRIV(curLight), &hitRecord).r;
    else
      ratio = material->shade(material, &hitRecord, PRIV(curLight));
  }
  PRIV(curLightBuf)->offsetSet(PRIV(curLightBuf), xy, ratio);
}

static void		lightUpdateCurrentState(LightRenderPassClass *self)
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

static void	lightRenderPassApply(LightRenderPassClass *self)
{
  VectorClass*  lightVector;
  AmbientClass	*ambientLight;
  int		nLights;

  RenderingSceneGetSingletonPtr()->getCurrentState()\
    ->reset(RenderingSceneGetSingletonPtr()->getCurrentState());
  lightVector = RenderingSceneGetSingletonPtr()->getCurrentScene()->mLights;
  nLights = lightVector->size(lightVector);
  PRIV(passCount) = 0;
  while (PRIV(passCount) < nLights)
  {
    RENDERPASS(self)->init(self, lightVector->mPtrs[PRIV(passCount)]);
    RENDERPASS(self)->applyLoop(RENDERPASS(self));
    PRIV(passCount)++;
  }
  ambientLight = RenderingSceneGetSingletonPtr()->getCurrentScene()->mAmbient;
  PRIV(passCount) = -1;
  RENDERPASS(self)->init(self, ambientLight);
  RENDERPASS(self)->applyLoop(RENDERPASS(self));
}

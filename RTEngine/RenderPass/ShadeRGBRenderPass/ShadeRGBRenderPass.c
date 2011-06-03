#include <string.h>

#include "ShadeRGBRenderPass.h"
#include "Color.h"
#include "RenderingScene.h"
#include "RTEngine.h"
#include "Maths.h"
#include "PointLight.h"
#include "TracerManager.h"
#include "Raycast.h"
#include "Sphere.h"
#include "Utils.h"

static void     ShadeRGBRenderPassConstructor();
static void     ShadeRGBRenderPassDestructor(ShadeRGBRenderPassClass* self);
static void	shadeRGBInit(ShadeRGBRenderPassClass *self);
static void	shadeRGBApplyPixel(ShadeRGBRenderPassClass *self, int i);
static void	shadeRGBApplyLoop(ShadeRGBRenderPassClass* self);
static void	shadeRGBUpdateCurrentState(ShadeRGBRenderPassClass *self);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static ShadeRGBRenderPassClass       _descr = { /* ShadeRGBRenderPassClass */
  { /* RenderPassClass */
    { /* Class */
      sizeof(ShadeRGBRenderPassClass),
      __RTTI_SHADERGBRENDERPASS, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
      &ShadeRGBRenderPassConstructor,
      &ShadeRGBRenderPassDestructor,
    },
    &shadeRGBInit,
    0, /* apply */
    VOID(&shadeRGBApplyLoop), /* applyLoop */
    0,
    0,
    VOID(&shadeRGBUpdateCurrentState),
  },
  {
    0, /* mMatBuffer */
    0, /* mReflectBuffer */
    0 /* mCurrentState */
  }
};

Class* ShadeRGBRenderPass = (Class*) &_descr;

static void     ShadeRGBRenderPassConstructor(ShadeRGBRenderPassClass* self,
                                         va_list* list)
{
  int		width;
  int		height;

  RenderPass->__constructor__(self, list);
  width = RTEngineGetSingletonPtr()->getSceneWidth();
  height = RTEngineGetSingletonPtr()->getSceneHeight();
  self->private.mBuffer = new(ColorBuffer, width, height);
  RENDERPASS(self)->applyLoop = VOID(&shadeRGBApplyLoop);
}

static void     ShadeRGBRenderPassDestructor(ShadeRGBRenderPassClass* self)
{
  RenderPass->__destructor__(self);
}

void		*shadeRGBRenderPassUnserialize(SceneSerializerClass *serializer,
                                            xmlNodePtr node)
{
  ShadeRGBRenderPassClass	*self;

  self = new(ShadeRGBRenderPass);
  if (renderPassUnserializeSet(serializer, node, self) != 0)
    return (0);
  return (self);
}

static void	shadeRGBInit(ShadeRGBRenderPassClass *self)
{
  GBufferClass	*gBuffer;

  gBuffer = RenderingSceneGetSingletonPtr()->getGBuffer();
  self->private.mMatBuffer =
    MATERIALBUF(gBuffer->getBuffer(gBuffer, RTTI(MaterialBuffer)));
  self->private.mCurrentState =
    RenderingSceneGetSingletonPtr()->getCurrentState();
}

static void		shadeRGBApplyPixel(ShadeRGBRenderPassClass *self, int i)
{
  MaterialClass		*material;
  Color			pixelColor;
  HitRecordStruct	hitRecord;

  material = self->private.mMatBuffer->getMaterial(self->private.mMatBuffer,
    i);
  if (!material)
    return ;
  buildHitRecordFromGBufferWithLight(&hitRecord, 0, i);
  pixelColor = material->shadeRGB(material, &hitRecord);
  self->private.mBuffer\
    ->offsetSet(self->private.mBuffer, i, pixelColor);
}

static void	shadeRGBApplyLoop(ShadeRGBRenderPassClass* self)
{
  int		i;

  i = 0;
  while (i < RTEngineGetSingletonPtr()->getSceneHeight()
           * RTEngineGetSingletonPtr()->getSceneWidth())
  {
    shadeRGBApplyPixel(self, i);
    i++;
  }
  shadeRGBUpdateCurrentState(self);
}

static void		shadeRGBUpdateCurrentState(ShadeRGBRenderPassClass *self)
{
  int			i;
  Color			*color;

  i = 0;
  while (i < RTEngineGetSingletonPtr()->getSceneHeight()
           * RTEngineGetSingletonPtr()->getSceneWidth())
  {
    color = self->private.mBuffer\
            ->getColorPtr(self->private.mBuffer, i);
    self->private.mCurrentState\
      ->offsetSet(self->private.mCurrentState, i, *color);
    i++;
  }
}

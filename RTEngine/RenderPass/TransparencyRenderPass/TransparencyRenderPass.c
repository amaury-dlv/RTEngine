#include <string.h>

#include "TransparencyRenderPass.h"
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
#include "Bump.h"
#include "Checker.h"

static void     TransparencyRenderPassConstructor();
static void     TransparencyRenderPassDestructor();
static void	transparencyInit(TransparencyRenderPassClass *self);
static void	transparencyApplyPixel(TransparencyRenderPassClass* self,
                                       int x, int y);
static void	transparencyUpdateCurrentState(TransparencyRenderPassClass *s);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static TransparencyRenderPassClass       _descr = { /* TransparencyRenderPassClass */
  { /* RenderPassClass */
    { /* Class */
      sizeof(TransparencyRenderPassClass),
      __RTTI_TRANSPARENCYRENDERPASS, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
      &TransparencyRenderPassConstructor,
      &TransparencyRenderPassDestructor,
    },
    &transparencyInit,
    0, /* apply */
    0, /* applyLoop */
    VOID(&transparencyApplyPixel),
    0,
    VOID(&transparencyUpdateCurrentState),
  },
  {
    0, /* mMatBuffer */
    0, /* mReflectBuffer */
    0, /* mIsRelfective */
    0 /* mCurrentState */
  }
};

Class* TransparencyRenderPass = (Class*) &_descr;

static void     TransparencyRenderPassConstructor(
  TransparencyRenderPassClass *self, va_list *list)
{
  int		width;
  int		height;

  RenderPass->__constructor__(self, list);
  width = RTEngineGetSingletonPtr()->getSceneWidth();
  height = RTEngineGetSingletonPtr()->getSceneHeight();
  self->private.mTransBuffer = new(ColorBuffer, width, height);
  self->private.mIsTransparent =
    xmalloc((sizeof(*self->private.mIsTransparent)
        * width * height + 1) / 8);
  memset(self->private.mIsTransparent,
         0,
         (sizeof(*self->private.mIsTransparent)
         * width * height + 1) / 8);
}

static void     TransparencyRenderPassDestructor(
  TransparencyRenderPassClass *self)
{
  RenderPass->__destructor__(self);
}

void		*transparencyRenderPassUnserialize(
  SceneSerializerClass *serializer, xmlNodePtr node)
{
  TransparencyRenderPassClass	*self;

  self = new(TransparencyRenderPass);
  if (renderPassUnserializeSet(serializer, node, self) != 0)
    return (0);
  return (self);
}

static void	transparencyInit(TransparencyRenderPassClass *self)
{
  GBufferClass	*gBuffer;

  gBuffer = RenderingSceneGetSingletonPtr()->getGBuffer();
  self->private.mMatBuffer =
    MATERIALBUF(gBuffer->getBuffer(gBuffer, RTTI(MaterialBuffer)));
  self->private.mCurrentState =
    RenderingSceneGetSingletonPtr()->getCurrentState();
}

static void		transparencyApplyPixel(
  TransparencyRenderPassClass *self, int x, int y)
{
  MaterialClass		*material;
  Color			pixelColor;
  HitRecordStruct	hitRecord;
  int			i;

  i = x + y * RTEngineGetSingletonPtr()->getSceneWidth();
  material = self->private.mMatBuffer->getMaterial(self->private.mMatBuffer,
                                                   i);
  if (!material
      || (RTTI(material) != RTTI(Transparent)
        && RTTI(material) != RTTI(Bump)
        && RTTI(material) != RTTI(Checker)))
    return ;
  SET_TRANS(i);
  buildHitRecordFromGBufferWithLight(&hitRecord, 0, i);
  pixelColor = material->shadeRGB(material, &hitRecord);
  self->private.mTransBuffer\
    ->offsetSet(self->private.mTransBuffer, i, pixelColor);
}

static void	transparencyUpdateCurrentState(
  TransparencyRenderPassClass *self)
{
  int		i;
  Color		*color;

  i = 0;
  while (i < RTEngineGetSingletonPtr()->getSceneHeight()
           * RTEngineGetSingletonPtr()->getSceneWidth())
  {
    if (IS_TRANS(i))
    {
      color = self->private.mTransBuffer\
              ->getColorPtr(self->private.mTransBuffer, i);
      self->private.mCurrentState\
        ->offsetSet(self->private.mCurrentState, i, *color);
    }
    i++;
  }
}

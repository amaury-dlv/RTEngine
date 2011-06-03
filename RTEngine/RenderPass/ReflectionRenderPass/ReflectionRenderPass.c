#include <string.h>

#include "RTTI.h"
#include "ReflectionRenderPass.h"
#include "Color.h"
#include "RenderingScene.h"
#include "RTEngine.h"
#include "Maths.h"
#include "PointLight.h"
#include "TracerManager.h"
#include "Raycast.h"
#include "Sphere.h"
#include "Utils.h"
#include "Reflective.h"
#include "GlossyReflective.h"

static void     ReflectionRenderPassConstructor();
static void     ReflectionRenderPassDestructor();
static void	reflectionInit(ReflectionRenderPassClass *self);
static void	reflectionApplyPixel(ReflectionRenderPassClass* self,
  int x, int y);
static void	reflectionUpdateCurrentState(ReflectionRenderPassClass *self);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static ReflectionRenderPassClass       _descr = { /* ReflectionRenderPassClass */
  { /* RenderPassClass */
    { /* Class */
      sizeof(ReflectionRenderPassClass),
      __RTTI_REFLECTIONRENDERPASS, /* RTTI */
      &ReflectionRenderPassConstructor,
      &ReflectionRenderPassDestructor,
    },
    &reflectionInit,
    0, /* apply */
    0, /* applyLoop */
    VOID(&reflectionApplyPixel), /* applyPixel */
    0,
    VOID(&reflectionUpdateCurrentState),
  },
  {
    0, /* mMatBuffer */
    0, /* mReflectBuffer */
    0, /* mIsRelfective */
    0 /* mCurrentState */
  }
};

Class* ReflectionRenderPass = (Class*) &_descr;

static void     ReflectionRenderPassConstructor(
                          ReflectionRenderPassClass* self,
                          va_list* list)
{
  int		width;
  int		height;

  RenderPass->__constructor__(self, list);
  width = RTEngineGetSingletonPtr()->getSceneWidth();
  height = RTEngineGetSingletonPtr()->getSceneHeight();
  self->private.mReflectBuffer = new(ColorBuffer, width, height);
  self->private.mIsReflective =
    xmalloc((sizeof(*self->private.mIsReflective)
        * width * height + 1) / 8);
  memset(self->private.mIsReflective,
         0,
         (sizeof(*self->private.mIsReflective)
         * width * height + 1) / 8);
}

static void     ReflectionRenderPassDestructor(ReflectionRenderPassClass* self)
{
  RenderPass->__destructor__(self);
}

void		*reflectionRenderPassUnserialize(SceneSerializerClass *serializer,
                                                 xmlNodePtr node)
{
  ReflectionRenderPassClass	*self;

  self = new(ReflectionRenderPass);
  if (renderPassUnserializeSet(serializer, node, self) != 0)
    return (0);
  return (self);
}

static void	reflectionInit(ReflectionRenderPassClass *self)
{
  GBufferClass	*gBuffer;

  gBuffer = RenderingSceneGetSingletonPtr()->getGBuffer();
  self->private.mMatBuffer =
    MATERIALBUF(gBuffer->getBuffer(gBuffer, RTTI(MaterialBuffer)));
  self->private.mCurrentState =
    RenderingSceneGetSingletonPtr()->getCurrentState();
}

static void		reflectionApplyPixel(ReflectionRenderPassClass *self,
  int x, int y)
{
  MaterialClass		*material;
  Color			pixelColor;
  HitRecordStruct	hitRecord;
  int			i;

  i = x + y * RTEngineGetSingletonPtr()->getSceneWidth();
  material = self->private.mMatBuffer->getMaterial(self->private.mMatBuffer,
   i);
  if (!material
      || (RTTI(material) != RTTI(Reflective)
          && RTTI(material) != RTTI(GlossyReflective)))
    return ;
  SET_REFL(i);
  buildHitRecordFromGBufferWithLight(&hitRecord, 0, i);
  pixelColor = material->shadeRGB(material, &hitRecord);
  self->private.mReflectBuffer\
    ->offsetSet(self->private.mReflectBuffer, i, pixelColor);
}

static void	reflectionUpdateCurrentState(ReflectionRenderPassClass *self)
{
  int		i;
  Color		*color;

  i = 0;
  while (i < RTEngineGetSingletonPtr()->getSceneHeight()
           * RTEngineGetSingletonPtr()->getSceneWidth())
  {
    if (IS_REFL(i))
    {
      color = self->private.mReflectBuffer\
              ->getColorPtr(self->private.mReflectBuffer, i);
      self->private.mCurrentState\
        ->offsetSet(self->private.mCurrentState, i, *color);
    }
    i++;
  }
}

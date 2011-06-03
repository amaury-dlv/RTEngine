#include <string.h>

#include "RTTI.h"
#include "BumpRenderPass.h"
#include "NormalBuffer.h"
#include "PerlinNoise.h"
#include "HitRecord.h"
#include "Color.h"
#include "RenderingScene.h"
#include "RTEngine.h"
#include "Maths.h"
#include "PointLight.h"
#include "TracerManager.h"
#include "Raycast.h"
#include "Sphere.h"
#include "Utils.h"
#include "Bump.h"

static void     BumpRenderPassConstructor();
static void     BumpRenderPassDestructor();
static void	bumpInit(BumpRenderPassClass *self);
static void	bumpApplyPixel(BumpRenderPassClass* self, int x, int y);
static void	bumpUpdateCurrentState(BumpRenderPassClass *self);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static BumpRenderPassClass       _descr = { /* BumpRenderPassClass */
  { /* RenderPassClass */
    { /* Class */
      sizeof(BumpRenderPassClass),
      __RTTI_BUMPRENDERPASS, /* RTTI */
      &BumpRenderPassConstructor,
      &BumpRenderPassDestructor,
    },
    &bumpInit,
    0, /* apply */
    0, /* applyLoop */
    VOID(&bumpApplyPixel), /* applyPixel */
    0,
    VOID(&bumpUpdateCurrentState),
  },
  {
    0, /* mMatBuffer */
    0, /* mNormalBuffer */
    0, /* mBumpBuffer */
    0 /* mCurrentState */
  }
};

Class* BumpRenderPass = (Class*) &_descr;

static void     BumpRenderPassConstructor(BumpRenderPassClass* self,
                                         va_list* list)
{
  int		width;
  int		height;

  RenderPass->__constructor__(self, list);
  width = RTEngineGetSingletonPtr()->getSceneWidth();
  height = RTEngineGetSingletonPtr()->getSceneHeight();
  self->private.mBumpBuffer = new(ColorBuffer, width, height);
}

static void     BumpRenderPassDestructor(BumpRenderPassClass* self)
{
  RenderPass->__destructor__(self);
}

void		*bumpRenderPassUnserialize(SceneSerializerClass *serializer,
                                                 xmlNodePtr node)
{
  BumpRenderPassClass	*self;

  self = new(BumpRenderPass);
  if (renderPassUnserializeSet(serializer, node, self) != 0)
    return (0);
  return (self);
}

static void	bumpInit(BumpRenderPassClass *self)
{
  GBufferClass	*gBuffer;

  gBuffer = RenderingSceneGetSingletonPtr()->getGBuffer();
  self->private.mMatBuffer =
    MATERIALBUF(gBuffer->getBuffer(gBuffer, RTTI(MaterialBuffer)));
  BUMPRPASS(self)->private.mNormalBuffer =
    VOID(gBuffer->getBuffer(gBuffer, __RTTI_NORMALBUFFER));
  self->private.mCurrentState =
    RenderingSceneGetSingletonPtr()->getCurrentState();
}

static void		bumpApplyPixel(BumpRenderPassClass *self, int x, int y)
{
  MaterialClass		*material;
  HitRecordStruct	hitRecord;
  Vector3D		*normalPtr;
  int			i;

  i = x + y * RTEngineGetSingletonPtr()->getSceneWidth();
  material = self->private.mMatBuffer->\
             getMaterial(self->private.mMatBuffer, i);
  if (!material || RTTI(material) != RTTI(Bump))
    return ;
  normalPtr = self->private.mNormalBuffer->\
              getNormalPtr(self->private.mNormalBuffer, i);
  if (BUMP(material)->bumpNormal)
    BUMP(material)->bumpNormal(BUMP(self), &hitRecord, normalPtr);
  buildHitRecordFromGBufferWithLight(&hitRecord, 0, i);
}

static void		bumpUpdateCurrentState(BumpRenderPassClass *self)
{
}

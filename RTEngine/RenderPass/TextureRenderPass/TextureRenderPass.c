#include <string.h>

#include "RTTI.h"
#include "TextureRenderPass.h"
#include "Color.h"
#include "RenderingScene.h"
#include "RTEngine.h"
#include "Maths.h"
#include "PointLight.h"
#include "TracerManager.h"
#include "Raycast.h"
#include "Sphere.h"
#include "Utils.h"
#include "Checker.h"
#include "Noise.h"
#include "ImageTexture.h"

static void     TextureRenderPassConstructor();
static void     TextureRenderPassDestructor();
static void	textureInit(TextureRenderPassClass *self);
static void	textureApplyPixel(TextureRenderPassClass* self, int x, int y);
static void	textureUpdateCurrentState(TextureRenderPassClass *self);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static TextureRenderPassClass       _descr = { /* TextureRenderPassClass */
  { /* RenderPassClass */
    { /* Class */
      sizeof(TextureRenderPassClass),
      __RTTI_TEXTURERENDERPASS, /* RTTI */
      &TextureRenderPassConstructor,
      &TextureRenderPassDestructor,
    },
    &textureInit,
    0, /* apply */
    0, /* applyLoop */
    VOID(&textureApplyPixel), /* applyPixel */
    0,
    VOID(&textureUpdateCurrentState),
  },
  {
    0, /* mMatBuffer */
    0, /* mTextureBuffer */
    0, /* mIsTexture */
    0 /* mCurrentState */
  }
};

Class* TextureRenderPass = (Class*) &_descr;

static void     TextureRenderPassConstructor(TextureRenderPassClass* self,
                                         va_list* list)
{
  int		width;
  int		height;

  RenderPass->__constructor__(self, list);
  width = RTEngineGetSingletonPtr()->getSceneWidth();
  height = RTEngineGetSingletonPtr()->getSceneHeight();
  self->private.mTextureBuffer = new(ColorBuffer, width, height);
  self->private.mIsTexture =
    xmalloc((sizeof(*self->private.mIsTexture)
        * width * height + 1) / 8);
  memset(self->private.mIsTexture,
         0,
         (sizeof(*self->private.mIsTexture)
         * width * height + 1) / 8);
}

static void     TextureRenderPassDestructor(TextureRenderPassClass* self)
{
  RenderPass->__destructor__(self);
}

void		*textureRenderPassUnserialize(SceneSerializerClass *serializer,
                                                 xmlNodePtr node)
{
  TextureRenderPassClass	*self;

  self = new(TextureRenderPass);
  if (renderPassUnserializeSet(serializer, node, self) != 0)
    return (0);
  return (self);
}

static void	textureInit(TextureRenderPassClass *self)
{
  GBufferClass	*gBuffer;

  gBuffer = RenderingSceneGetSingletonPtr()->getGBuffer();
  self->private.mMatBuffer =
    MATERIALBUF(gBuffer->getBuffer(gBuffer, RTTI(MaterialBuffer)));
  self->private.mCurrentState =
    RenderingSceneGetSingletonPtr()->getCurrentState();
}

static void		textureApplyPixel(TextureRenderPassClass *self,
  int x, int y)
{
  MaterialClass		*material;
  Color			pixelColor;
  HitRecordStruct	hitRecord;
  int			i;

  i = x + y * RTEngineGetSingletonPtr()->getSceneWidth();
  material = self->private.mMatBuffer->getMaterial(self->private.mMatBuffer,
    i);
  if (!material ||
      (RTTI(material) != RTTI(Checker)
       && RTTI(material) != RTTI(Noise)
       && RTTI(material) !=  RTTI(ImageTexture)))
    return ;
  SET_TEX(i);
  buildHitRecordFromGBufferWithLight(&hitRecord, 0, i);
  pixelColor = material->shadeRGB(material, &hitRecord);
  self->private.mTextureBuffer\
    ->offsetSet(self->private.mTextureBuffer, i, pixelColor);
}

static void		textureUpdateCurrentState(TextureRenderPassClass *self)
{
  int			i;
  Color			*color;

  i = 0;
  while (i < RTEngineGetSingletonPtr()->getSceneHeight()
           * RTEngineGetSingletonPtr()->getSceneWidth())
  {
    if (IS_TEX(i))
    {
      color = self->private.mTextureBuffer\
              ->getColorPtr(self->private.mTextureBuffer, i);
      self->private.mCurrentState\
        ->offsetSet(self->private.mCurrentState, i, *color);
    }
    i++;
  }
}

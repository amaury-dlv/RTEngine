#include <string.h>

#include "RTTI.h"
#include "ToneMappingRenderPass.h"
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

static void     toneMappingRenderPassConstructor();
static void     toneMappingRenderPassDestructor();
static void	toneMappingRenderPassInit(ToneMappingRenderPassClass *self);
static void	toneMappingRenderPassPixel(ToneMappingRenderPassClass* self,
                                           int x, int y);
static void	toneMappingRenderPassUpdateCurrentState(RenderPassClass *self);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static ToneMappingRenderPassClass       _descr = { /* TMRenderPassClass */
  { /* RenderPassClass */
    { /* Class */
      sizeof(ToneMappingRenderPassClass),
      __RTTI_TONEMAPPINGRENDERPASS, /* RTTI */
      &toneMappingRenderPassConstructor,
      &toneMappingRenderPassDestructor,
    },
    &toneMappingRenderPassInit,
    0, /* apply */
    0, /* applyLoop */
    VOID(&toneMappingRenderPassPixel), /* renderPassPixel (pure virtual) */
    0,
    VOID(&toneMappingRenderPassUpdateCurrentState),
  },
  { /* private */
    0, /* mColorBuffer */
    0 /* mCoeff */
  }
};

Class* ToneMappingRenderPass = (Class*) &_descr;

static void     toneMappingRenderPassConstructor(ToneMappingRenderPassClass*
    						 self, va_list* list)
{
  RenderPass->__constructor__(self, list);
}

static void     toneMappingRenderPassDestructor(ToneMappingRenderPassClass*
    						self)
{
  RenderPass->__destructor__(self);
}

void	*toneMappingRenderPassUnserialize(SceneSerializerClass *serializer,
                                                    xmlNodePtr node)
{
  ToneMappingRenderPassClass	*self;
  char				*coeff;

  self = new(ToneMappingRenderPass);
  if (renderPassUnserializeSet(serializer, node, self) != 0)
    return (0);
  serializer->setStrsFromProp(serializer, node, "coeff", &coeff, NULL);
  if (setFloatFromStr(coeff, &self->private.mCoeff) == -1)
  {
    fprintf(stderr, "Warning: Tone Mapping set to default value %d\n",
        DEFAULT_TONE_MAPPING);
    self->private.mCoeff = DEFAULT_TONE_MAPPING;
  }
  return (self);
}

static void	toneMappingRenderPassInit(ToneMappingRenderPassClass *self)
{
  self->private.mColorBuffer =
    RenderingSceneGetSingletonPtr()->getCurrentState();
}

static void	toneMappingRenderPassPixel(ToneMappingRenderPassClass* self,
                                           int x, int y)
{
  int		i;
  Color		*current;

  i = x + y * RTEngineGetSingletonPtr()->getSceneWidth();
  current = self->private.mColorBuffer->getColorPtr(self->private.mColorBuffer,
                                                    i);
  current->r = current->r / (current->r + self->private.mCoeff);
  current->g = current->g / (current->g + self->private.mCoeff);
  current->b = current->b / (current->b + self->private.mCoeff);
}

static void	toneMappingRenderPassUpdateCurrentState(RenderPassClass
    							*self)
{
}

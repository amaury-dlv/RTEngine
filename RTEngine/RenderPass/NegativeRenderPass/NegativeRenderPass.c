#include <string.h>

#include "RTTI.h"
#include "NegativeRenderPass.h"
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

static void     negativeRenderPassConstructor();
static void     negativeRenderPassDestructor(NegativeRenderPassClass* self);
static void	negativeRenderPassInit(NegativeRenderPassClass *self);
static void	negativeRenderPassPixel(NegativeRenderPassClass* self,
  int x, int y);
static void	negativeRenderPassUpdateCurrentState(RenderPassClass *self);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static NegativeRenderPassClass       _descr = { /* NegativeRenderPassClass */
  { /* RenderPassClass */
    { /* Class */
      sizeof(NegativeRenderPassClass),
      __RTTI_NEGATIVERENDERPASS, /* RTTI */
      &negativeRenderPassConstructor,
      &negativeRenderPassDestructor,
    },
    &negativeRenderPassInit,
    0, /* apply */
    0, /* applyLoop */
    VOID(&negativeRenderPassPixel), /* renderPassPixel (pure virtual) */
    0,
    VOID(&negativeRenderPassUpdateCurrentState),
  },
  { /* private */
    0 /* mColorBuffer */
  }
};

Class* NegativeRenderPass = (Class*) &_descr;

static void     negativeRenderPassConstructor(NegativeRenderPassClass* self,
                                             va_list* list)
{
  RenderPass->__constructor__(self, list);
}

static void     negativeRenderPassDestructor(NegativeRenderPassClass* self)
{
  RenderPass->__destructor__(self);
}

void	*negativeRenderPassUnserialize(SceneSerializerClass *serializer,
                                                    xmlNodePtr node)
{
  NegativeRenderPassClass	*self;

  self = new(NegativeRenderPass);
  if (renderPassUnserializeSet(serializer, node, self) != 0)
    return (0);
  return (self);
}

static void		negativeRenderPassInit(NegativeRenderPassClass *self)
{
  self->private.mColorBuffer =
    RenderingSceneGetSingletonPtr()->getCurrentState();
}

static void	negativeRenderPassPixel(NegativeRenderPassClass* self,
  int x, int y)
{
  int		i;
  Color		*current;

  i = x + y * RTEngineGetSingletonPtr()->getSceneWidth();
  current = self->private.mColorBuffer->getColorPtr(self->private.mColorBuffer,
    i);
  current->r = 1. - MIN(current->r, 1);
  current->g = 1. - MIN(current->g, 1);
  current->b = 1. - MIN(current->b, 1);
}

static void	negativeRenderPassUpdateCurrentState(RenderPassClass *self)
{
}

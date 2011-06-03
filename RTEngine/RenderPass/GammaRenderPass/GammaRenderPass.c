#include <string.h>

#include "RTTI.h"
#include "GammaRenderPass.h"
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

static void     gammaRenderPassConstructor();
static void     gammaRenderPassDestructor();
static void	gammaRenderPassInit(GammaRenderPassClass *self);
static void	gammaRenderPassPixel(GammaRenderPassClass* self, int x, int y);
static void	gammaRenderPassUpdateCurrentState(RenderPassClass *self);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static GammaRenderPassClass       _descr = { /* GammaRenderPassClass */
  { /* RenderPassClass */
    { /* Class */
      sizeof(GammaRenderPassClass),
      __RTTI_GAMMARENDERPASS, /* RTTI */
      &gammaRenderPassConstructor,
      &gammaRenderPassDestructor,
    },
    &gammaRenderPassInit,
    0, /* apply */
    0, /* applyLoop */
    VOID(&gammaRenderPassPixel), /* renderPassPixel (pure virtual) */
    0,
    VOID(&gammaRenderPassUpdateCurrentState),
  },
  { /* private */
    0, /* mColorBuffer */
    0 /* mE */
  }
};

Class* GammaRenderPass = (Class*) &_descr;

static void     gammaRenderPassConstructor(GammaRenderPassClass* self,
                                             va_list* list)
{
  RenderPass->__constructor__(self, list);
}

static void     gammaRenderPassDestructor(GammaRenderPassClass* self)
{
  RenderPass->__destructor__(self);
}

void		  	*gammaRenderPassUnserialize(SceneSerializerClass *serializer,
                                                    xmlNodePtr node)
{
  GammaRenderPassClass	*self;
  char			*e;

  self = new(GammaRenderPass);
  if (renderPassUnserializeSet(serializer, node, self) != 0)
    return (0);
  serializer->setStrsFromProp(serializer, node, "e", &e, NULL);
  if (setFloatFromStr(e, &self->private.mE) == -1)
  {
    fprintf(stderr, "Warning: Gamma set to default value %d\n",
        DEFAULT_GAMMA);
    self->private.mE = DEFAULT_GAMMA;
  }
  self->private.mE = 1 / self->private.mE;
  return (self);
}

static void	gammaRenderPassInit(GammaRenderPassClass *self)
{
  self->private.mColorBuffer =
    RenderingSceneGetSingletonPtr()->getCurrentState();
}

static void		gammaRenderPassPixel(GammaRenderPassClass* self,
    					     int x, int y)
{
  int			i;
  Color			*current;

  i = x + y * RTEngineGetSingletonPtr()->getSceneWidth();
  current =
    self->private.mColorBuffer->getColorPtr(self->private.mColorBuffer, i);
  current->r = powf(current->r, self->private.mE);
  current->g = powf(current->g, self->private.mE);
  current->b = powf(current->b, self->private.mE);
}

static void	gammaRenderPassUpdateCurrentState(RenderPassClass
    						  *self)
{
}

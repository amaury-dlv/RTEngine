#include <math.h>

#include "DOFRenderPass.h"
#include "RenderingScene.h"
#include "Color.h"
#include "RTEngine.h"
#include "Maths.h"

static void     DOFRenderPassConstructor();
static void     DOFRenderPassDestructor();
static void	DOFRenderPassInit(DOFRenderPassClass *self);
static void	DOFRenderPassBlurPixel(DOFRenderPassClass *self,
                                       float treshold, Point2D p,
                                       Color *color);
static void	DOFRenderPassPixel(DOFRenderPassClass* self, int x, int y);
static void	DOFRenderPassUpdateCurrentState(DOFRenderPassClass *self);
static float	DOFRenderPassBlurPixelColor(DOFRenderPassClass *self,
  Color *neigh,
  Color *color);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static DOFRenderPassClass       _descr = { /* DOFRenderPassClass */
  { /* RenderPassClass */
    { /* Class */
      sizeof(DOFRenderPassClass),
      0, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
      &DOFRenderPassConstructor,
      &DOFRenderPassDestructor,
    },
    &DOFRenderPassInit,
    0, /* apply */
    0, /* applyLoop */
    VOID(&DOFRenderPassPixel), /* renderPassPixel (pure virtual) */
    0,
    VOID(&DOFRenderPassUpdateCurrentState),
  },
  { /* private */
    0, /* mColorBlurBuffer */
    0, /* mDepthBuffer */
    0 /* mCurrentState */
  }
};

Class* DOFRenderPass = (Class*) &_descr;

static void     DOFRenderPassConstructor(DOFRenderPassClass* self,
                                         va_list* list)
{
  int		width;
  int		height;

  RenderPass->__constructor__(self, list);
  width = RTEngineGetSingletonPtr()->getSceneWidth();
  height = RTEngineGetSingletonPtr()->getSceneHeight();
  self->private.mColorBlurBuffer = new(ColorBuffer, width, height);
}

static void     DOFRenderPassDestructor(DOFRenderPassClass* self)
{
  RenderPass->__destructor__(self);
}

static void	DOFRenderPassInit(DOFRenderPassClass *self)
{
  GBufferClass	*gBuffer;

  gBuffer = RenderingSceneGetSingletonPtr()->getGBuffer();
  self->private.mDepthBuffer =
    DEPTHBUFFER(gBuffer->getBuffer(gBuffer, __RTTI_DEPTHBUFFER));
  self->private.mCurrentState = RenderingSceneGetSingletonPtr()\
                                ->getCurrentState();
}

/* vvv */

static float	DOFRenderPassBlurPixelColor(DOFRenderPassClass *self,
  Color *neigh,
  Color *color)
{
  float		l;

  l = 0.3 * neigh->r + 0.59 * neigh->g + 0.11 * neigh->b;
  color->r += neigh->r * l;
  color->g += neigh->g * l;
  color->b += neigh->b * l;
  return (l);
}

static void	DOFRenderPassBlurPixel(DOFRenderPassClass *self,
                                       float treshold, Point2D p, Color *color)
{
  int		numpix;
  Point2D	b;
  Point2D	e;
  Color		*neigh;
  float		tmp;

  tmp = 0;
  numpix = ceil(treshold);
  e.y = MIN(p.y + numpix, IMAGEBUF(PRIV(mCurrentState))->height - 1);
  e.x = MIN(p.x + numpix, IMAGEBUF(PRIV(mCurrentState))->width - 1);
  b.y = MAX(0, p.y - numpix);
  while (b.y <= e.y)
  {
    b.x = MAX(0, p.x - numpix);
    while (b.x <= e.x)
    {
      neigh = PRIV(mCurrentState)->getColorPtr(PRIV(mCurrentState),
          b.y * IMAGEBUF(PRIV(mCurrentState))->width + b.x);
      tmp += DOFRenderPassBlurPixelColor(self, neigh, color);
      b.x++;
    }
    b.y++;
  }
  colorMult(color, (tmp == 0) ? 0 : (1 / tmp));
}

static void	DOFRenderPassPixel(DOFRenderPassClass* self, int x, int y)
{
  Color		color;
  int		i;
  float		depth;

  i = y * IMAGEBUF(PRIV(mCurrentState))->width + x;
  depth = self->private.mDepthBuffer->getDepth(self->private.mDepthBuffer, i);
  color = PRIV(mCurrentState)->getColor(PRIV(mCurrentState),
      y * IMAGEBUF(PRIV(mCurrentState))->width + x);
  DOFRenderPassBlurPixel(self, 30. / MAX(1., ABS(20. - depth)),
   (Point2D){x, y}, &color);
  PRIV(mColorBlurBuffer)->offsetSet(PRIV(mColorBlurBuffer),
    y * IMAGEBUF(PRIV(mColorBlurBuffer))->width + x, color);
}

/* ^^^ */

static void	DOFRenderPassUpdateCurrentState(DOFRenderPassClass *self)
{
  int		i;
  int		size;
  Color		*newColor;

  i = 0;
  size = RTEngineGetSingletonPtr()->getSceneWidth()
         * RTEngineGetSingletonPtr()->getSceneHeight();
  while (i < size)
  {
    newColor = self->private.mColorBlurBuffer\
               ->getColorPtr(self->private.mColorBlurBuffer, i);
    PRIV(mCurrentState)->offsetSet(PRIV(mCurrentState), i,
        *newColor);
    i++;
  }
  printf("done\n");
}

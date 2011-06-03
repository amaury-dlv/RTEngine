#include <math.h>
#include "RTTI.h"
#include "LightBuffer.h"
#include "ColorBuffer.h"
#include "DepthBuffer.h"
#include "RenderingScene.h"
#include "Utils.h"
#include "Point2D.h"

static void     LightBufferConstructor(LightBufferClass* self, va_list* list);
static void     LightBufferDestructor(LightBufferClass* self);
static float	lightBufferGetLight(LightBufferClass* self, int i);
static void	lightBufferOffsetSet(LightBufferClass* self, int i,
                                     float light);
static void	lightBufferSet(LightBufferClass* self, int x, int y,
                               float light);
static void	lightBufferConverToColorBuffer(LightBufferClass* self,
                                               ColorBufferClass* colorBuffer);
static void	lightBufferBlur(LightBufferClass *self, int kernelSize);
static float	lightBufferBlurGetCoeff(LightBufferClass *self,
                                        int xy,
                                        DepthBufferClass *depthBuf,
                                        Point2D coord);
static void	lightBufferBlurKernelVer(LightBufferClass *self,
                                         int xy,
                                         DepthBufferClass *depthBuf,
                                         int kernelSize);
static void	lightBufferBlurKernelHor(LightBufferClass *self,
                                         int xy,
                                         DepthBufferClass *depthBuf,
                                         int kernelSize);
static void	lightBufferBlurWay(LightBufferClass *self, int kernelSize,
                                   DepthBufferClass *depthBuf, int secondPass);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static LightBufferClass       _descr = { /* LightBuffer */
  {/* ImageBuffer */
    { /* Class */
      sizeof(LightBufferClass),
      __RTTI_LIGHTBUFFER,
      &LightBufferConstructor,
      &LightBufferDestructor,
    },
    &lightBufferConverToColorBuffer,
    0, /* id */
    0, /* width */
    0, /* height */
    {
    }
  },
  &lightBufferGetLight,
  &lightBufferOffsetSet,
  &lightBufferSet,
  &lightBufferBlur,
  {
    0,
  }
};

Class* LightBuffer = (Class*) &_descr;

float	g_light_blur_weights[13] =
{
  0.002216,
  0.008764,
  0.026995,
  0.064759,
  0.120985,
  0.176033,
  0.199471,
  0.176033,
  0.120985,
  0.064759,
  0.026995,
  0.008764,
  0.002216,
};

static void     LightBufferConstructor(LightBufferClass* self,
                                             va_list* list)
{
  ImageBuffer->__constructor__(self, list);
  self->private.mBuffer =
    xmalloc(sizeof(*self->private.mBuffer)
        * IMAGEBUF(self)->width
        * IMAGEBUF(self)->height);
}

static void     LightBufferDestructor(LightBufferClass* self)
{
  free(self->private.mBuffer);
}

static float		lightBufferGetLight(LightBufferClass* self, int i)
{
  return (self->private.mBuffer[i]);
}

static void		lightBufferOffsetSet(LightBufferClass* self, int i,
                                             float light)
{
  self->private.mBuffer[i] = light;
}

static void		lightBufferSet(LightBufferClass* self, int x, int y,
                                       float light)
{
  int			i;

  i = y * IMAGEBUF(self)->width + x;
  self->private.mBuffer[i] = light;
}

static void	lightBufferConverToColorBuffer(LightBufferClass* self,
                                               ColorBufferClass* colorBuffer)
{
  int		i;
  float		c;

  i = 0;
  while (i < IMAGEBUF(self)->width * IMAGEBUF(self)->height)
  {
    c = self->private.mBuffer[i] * 256.;
    colorBuffer->set(colorBuffer,
        i % IMAGEBUF(colorBuffer)->width,
        i / IMAGEBUF(colorBuffer)->width,
        (Color) {c, c, c});
    i++;
  }
}

static float	lightBufferBlurGetCoeff(LightBufferClass *self, int xy,
                                        DepthBufferClass *depthBuf,
					Point2D coord)
{
  float		depth;
  float		depthPoint;

  depth = depthBuf->getDepth(depthBuf, xy);
  xy += coord.x;
  xy += coord.y * IMAGEBUF(self)->width;
  depthPoint = depthBuf->getDepth(depthBuf, xy);
  return (fabsf(depthPoint - depth) + 1.);
}

static void		lightBufferBlurKernelVer(LightBufferClass *self,
                                                 int xy,
                                                 DepthBufferClass *depthBuf,
						 int kernelSize)
{
  int		x;
  int		y;
  int		xyK;
  Point2D	coord;
  float		c;

  x = xy % IMAGEBUF(self)->width;
  y = xy / IMAGEBUF(self)->width;
  coord = (Point2D){0, -kernelSize};
  while (coord.y < kernelSize)
  {
    if (coord.y && y + coord.y >= 0 && y + coord.y < IMAGEBUF(self)->height)
    {
      xyK = (y + coord.y) * IMAGEBUF(self)->width + x;
      c = (1. - PRIV(mBuffer)[xyK])
        / lightBufferBlurGetCoeff(self, xy, depthBuf, coord);
      PRIV(mBuffer)[xy] -= c * g_light_blur_weights[(int)coord.y + 6];
      if (PRIV(mBuffer)[xy] < 0)
        PRIV(mBuffer)[xy] = 0;
    }
    ++coord.y;
  }
}

static void		lightBufferBlurKernelHor(LightBufferClass *self,
                                                 int xy,
                                                 DepthBufferClass *depthBuf,
						 int kernelSize)
{
  int		x;
  int		y;
  int		xyK;
  Point2D	coord;
  float		coeff;

  x = xy % IMAGEBUF(self)->width;
  y = xy / IMAGEBUF(self)->width;
  coord = (Point2D){-kernelSize, 0};
  while (coord.x < kernelSize)
  {
    if (coord.x && x + coord.x >= 0 && x + coord.x < IMAGEBUF(self)->width)
    {
      xyK = y * IMAGEBUF(self)->width + x + coord.x;
      coeff = (1. - PRIV(mBuffer)[xyK])
        / lightBufferBlurGetCoeff(self, xy, depthBuf, coord);
      PRIV(mBuffer)[xy] -= coeff * g_light_blur_weights[(int)coord.x + 6];
      if (PRIV(mBuffer)[xy] < 0)
        PRIV(mBuffer)[xy] = 0;
    }
    ++coord.x;
  }
}

static void	lightBufferBlurWay(LightBufferClass *self, int kernelSize,
                                   DepthBufferClass *depthBuf, int secondPass)
{
  int		x;
  int		y;
  int		xy;

  y = 0;
  xy = 0;
  while (y < IMAGEBUF(self)->height)
  {
    x = 0;
    while (x < IMAGEBUF(self)->width)
    {
      if (!secondPass)
        lightBufferBlurKernelHor(self, xy, depthBuf, kernelSize);
      else
        lightBufferBlurKernelVer(self, xy, depthBuf, kernelSize);
      ++xy;
      ++x;
    }
    ++y;
  }
}

static void		lightBufferBlur(LightBufferClass *self, int kernelSize)
{
  DepthBufferClass	*depthBuf;
  GBufferClass		*gBuffer;

  if (kernelSize > 6)
    kernelSize = 6;
  gBuffer = RenderingSceneGetSingletonPtr()->getGBuffer();
  depthBuf = DEPTHBUFFER(gBuffer->getBuffer(gBuffer, __RTTI_DEPTHBUFFER));
  lightBufferBlurWay(self, kernelSize, depthBuf, false);
  lightBufferBlurWay(self, kernelSize, depthBuf, true);
}

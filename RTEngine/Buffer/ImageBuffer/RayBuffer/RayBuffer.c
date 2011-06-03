#include "RTTI.h"
#include "RayBuffer.h"
#include "ColorBuffer.h"
#include "Utils.h"

static void     RayBufferConstructor(RayBufferClass *self, va_list *list);
static void     RayBufferDestructor(RayBufferClass *self);
static RayStruct	rayBufferGetRay(RayBufferClass *self, int i);
static RayStruct*	rayBufferGetRayPtr(RayBufferClass *self, int i);
static void	rayBufferSetFromStruct(RayBufferClass *self, int x, int y,
                                              RayStruct *rayClass);
static void	rayBufferconvertToColorBuffer(RayBufferClass *self,
                                              ColorBufferClass *colorBuffer);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static RayBufferClass       _descr = { /* RayBuffer */
  {/* ImageBuffer */
    { /* Class */
      sizeof(RayBufferClass),
      __RTTI_RAYBUFFER,
      &RayBufferConstructor,
      &RayBufferDestructor,
    },
    &rayBufferconvertToColorBuffer,
    0, /* id */
    0, /* width */
    0, /* height */
    {
    }
  },
  &rayBufferGetRay,
  &rayBufferGetRayPtr,
  &rayBufferSetFromStruct,
  {
    0,
  }
};

Class *RayBuffer = (Class*) &_descr;

static void     RayBufferConstructor(RayBufferClass *self,
                                             va_list *list)
{
  ImageBuffer->__constructor__(self, list);
  self->private.mBuffer =
    xmalloc(sizeof(*self->private.mBuffer)
        * IMAGEBUF(self)->width
        * IMAGEBUF(self)->height);
}

static void     RayBufferDestructor(RayBufferClass *self)
{
  free(self->private.mBuffer);
}

static RayStruct	rayBufferGetRay(RayBufferClass *self, int i)
{
  return (self->private.mBuffer[i]);
}

static RayStruct*	rayBufferGetRayPtr(RayBufferClass *self, int i)
{
  return (&self->private.mBuffer[i]);
}

static void		rayBufferSetFromStruct(RayBufferClass *self,
                                               int x, int y,
                                               RayStruct *rayClass)
{
  int			i;

  i = y * IMAGEBUF(self)->width + x;
  self->private.mBuffer[i] =
    (RayStruct) {rayClass->origin, rayClass->direction};
}

static void	rayBufferconvertToColorBuffer(RayBufferClass *self,
                                              ColorBufferClass *colorBuffer)
{
  int		i;
  float		r;
  float		g;
  float		b;

  i = 0;
  while (i < IMAGEBUF(self)->width * IMAGEBUF(self)->height)
  {
    r = (self->private.mBuffer[i].direction.x + 1) * .5f;
    g = (self->private.mBuffer[i].direction.y + 1) * .5f;
    b = (self->private.mBuffer[i].direction.z + 1) * .5f;
    colorBuffer->set(colorBuffer,
        i % IMAGEBUF(colorBuffer)->width,
        i / IMAGEBUF(colorBuffer)->width,
        (Color) {r, g, b});
    i++;
  }
}

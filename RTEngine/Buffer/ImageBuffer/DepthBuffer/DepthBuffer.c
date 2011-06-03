#include "RTTI.h"
#include "DepthBuffer.h"
#include "ColorBuffer.h"
#include "Maths.h"
#include "Utils.h"

static void     DepthBufferConstructor(DepthBufferClass* self, va_list* list);
static void     DepthBufferDestructor(DepthBufferClass* self);
static double	depthBufferGetDepth(DepthBufferClass* self, int i);
static void	depthBufferSet(DepthBufferClass* self,
                               int x, int y, double depth);
static void	depthBufferSetFromHitRecord(DepthBufferClass* self,
                                            int x, int y,
                                            HitRecordStruct* rec);
static void	depthBufferConvertToColorbuffer(DepthBufferClass* self,
                                                ColorBufferClass* colorBuffer);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static DepthBufferClass       _descr = { /* DepthBuffer */
  {/* ImageBuffer */
    { /* Class */
      sizeof(DepthBufferClass),
      __RTTI_DEPTHBUFFER,
      &DepthBufferConstructor,
      &DepthBufferDestructor,
    },
    &depthBufferConvertToColorbuffer,
    0, /* id */
    0, /* width */
    0, /* height */
    {
    }
  },
  &depthBufferGetDepth,
  &depthBufferSet,
  &depthBufferSetFromHitRecord,
  {
    0, /* mBuffer */
    0. /* mMaxDist */
  }
};

Class* DepthBuffer = (Class*) &_descr;

static void     DepthBufferConstructor(DepthBufferClass* self,
                                             va_list* list)
{
  ImageBuffer->__constructor__(self, list);
  self->private.mBuffer =
    xmalloc(sizeof(*self->private.mBuffer)
        * IMAGEBUF(self)->width
        * IMAGEBUF(self)->height);
  self->private.mMaxDist = 0;
}

static void     DepthBufferDestructor(DepthBufferClass* self)
{
  free(self->private.mBuffer);
}

static double	depthBufferGetDepth(DepthBufferClass* self, int i)
{
  return (self->private.mBuffer[i]);
}

static void	depthBufferSet(DepthBufferClass* self,
                               int x, int y, double depth)
{
  int		i;

  i = y * IMAGEBUF(self)->width + x;
  self->private.mBuffer[i] = depth;
}

static void	depthBufferSetFromHitRecord(DepthBufferClass* self,
                                            int x, int y,
                                            HitRecordStruct* rec)
{
  int		i;
  double	depth;

  depth = vectDist(&(rec->hitPoint), &(rec->ray.origin));
  i = y * IMAGEBUF(self)->width + x;
  self->private.mBuffer[i] = depth;
  if (depth > self->private.mMaxDist)
    self->private.mMaxDist = depth;
}

static void	depthBufferConvertToColorbuffer(DepthBufferClass* self,
                                                ColorBufferClass* colorBuffer)
{
  int		i;
  float		c;

  i = 0;
  while (i < IMAGEBUF(self)->width * IMAGEBUF(self)->height)
  {
    c = (1.- MIN((self->private.mBuffer[i] / (self->private.mMaxDist
              / 200)) , 1.)) * 1.f;
    colorBuffer->set(colorBuffer,
        i % IMAGEBUF(colorBuffer)->width,
        i / IMAGEBUF(colorBuffer)->width,
        (Color) {c, c, c});
    i++;
  }
}

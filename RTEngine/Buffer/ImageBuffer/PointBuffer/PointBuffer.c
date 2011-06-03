#include "RTTI.h"
#include "PointBuffer.h"
#include "Utils.h"
#include "ColorBuffer.h"

static void     	PointBufferConstructor();
static void     	PointBufferDestructor(PointBufferClass* self);
static Vector3D	pointBufferGetPoint(PointBufferClass* self, int i);
static Vector3D*	pointBufferGetPointPtr(PointBufferClass* self, int i);
static void		pointBufferSet(PointBufferClass* self, int x, int y,
                                       Point3D pointStruct);
static void	pointBufferConvertToColorBuffer(PointBufferClass* self,
                                                ColorBufferClass* colorBuffer);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static PointBufferClass       _descr = { /* PointBuffer */
  {/* ImageBuffer */
    { /* Class */
      sizeof(PointBufferClass),
      __RTTI_POINTBUFFER,
      &PointBufferConstructor,
      &PointBufferDestructor,
    },
    &pointBufferConvertToColorBuffer,
    0, /* id */
    0, /* width */
    0, /* height */
    {
    }
  },
  &pointBufferGetPoint,
  &pointBufferGetPointPtr,
  &pointBufferSet,
  {
    0,
  }
};

Class* PointBuffer = (Class*) &_descr;

static void     PointBufferConstructor(PointBufferClass* self,
                                             va_list* list)
{
  ImageBuffer->__constructor__(self, list);
  self->private.mBuffer =
    xmalloc(sizeof(*self->private.mBuffer)
        * IMAGEBUF(self)->width
        * IMAGEBUF(self)->height);
}

static void     PointBufferDestructor(PointBufferClass* self)
{
  free(self->private.mBuffer);
}

static Vector3D	pointBufferGetPoint(PointBufferClass* self, int i)
{
  return (self->private.mBuffer[i]);
}

static Vector3D*	pointBufferGetPointPtr(PointBufferClass* self, int i)
{
  return (&self->private.mBuffer[i]);
}

static void		pointBufferSet(PointBufferClass* self, int x, int y,
                                       Point3D pointStruct)
{
  int			i;

  i = y * IMAGEBUF(self)->width + x;
  self->private.mBuffer[i] = pointStruct;
}

static void	pointBufferConvertToColorBuffer(PointBufferClass* self,
                                                 ColorBufferClass* colorBuffer)
{
  int		i;
  float		r;
  float		g;
  float		b;

  i = 0;
  vectNormalize(&self->private.mBuffer[i]);
  while (i < IMAGEBUF(self)->width * IMAGEBUF(self)->height)
  {
    r = (self->private.mBuffer[i].x + 1.) * .5;
    g = (self->private.mBuffer[i].y + 1.) * .5;
    b = (self->private.mBuffer[i].z + 1.) * .5;
    colorBuffer->set(colorBuffer,
        i % IMAGEBUF(colorBuffer)->width,
        i / IMAGEBUF(colorBuffer)->width,
        (Color) {r, g, b});
    i++;
  }
}

#include "RTTI.h"
#include "NormalBuffer.h"
#include "ColorBuffer.h"
#include "Utils.h"

static void     	NormalBufferConstructor();
static void     	NormalBufferDestructor(NormalBufferClass* self);
static Vector3D	normalBufferGetNormal(NormalBufferClass* self, int i);
static Vector3D*	normalBufferGetNormalPtr(NormalBufferClass* self,
                                                 int i);
static void	normalBufferSet(NormalBufferClass* self, int x, int y,
                                        Vector3D vectStruct);
static void	normalBufferConvertToColorBuffer(NormalBufferClass* self,
                                                ColorBufferClass* colorBuffer);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static NormalBufferClass       _descr = { /* NormalBuffer */
  {/* ImageBuffer */
    { /* Class */
      sizeof(NormalBufferClass),
      __RTTI_NORMALBUFFER,
      &NormalBufferConstructor,
      &NormalBufferDestructor,
    },
    &normalBufferConvertToColorBuffer,
    0, /* id */
    0, /* width */
    0, /* height */
    {
    }
  },
  &normalBufferGetNormal,
  &normalBufferGetNormalPtr,
  &normalBufferSet,
  {
    0,
  }
};

Class* NormalBuffer = (Class*) &_descr;

static void     NormalBufferConstructor(NormalBufferClass* self,
                                             va_list* list)
{
  ImageBuffer->__constructor__(self, list);
  self->private.mBuffer =
    xmalloc(sizeof(*self->private.mBuffer)
        * IMAGEBUF(self)->width
        * IMAGEBUF(self)->height);
}

static void     NormalBufferDestructor(NormalBufferClass* self)
{
  free(self->private.mBuffer);
}

static Vector3D	normalBufferGetNormal(NormalBufferClass* self, int i)
{
  return (self->private.mBuffer[i]);
}

static Vector3D*	normalBufferGetNormalPtr(NormalBufferClass* self,
                                                 int i)
{
  return (&self->private.mBuffer[i]);
}

static void		normalBufferSet(NormalBufferClass* self, int x, int y,
                                        Vector3D vectStruct)
{
  int			i;

  i = y * IMAGEBUF(self)->width + x;
  self->private.mBuffer[i] = vectStruct;
}

/*
 * It assumes the whole normalBuffer is normalized
 */
static void	normalBufferConvertToColorBuffer(NormalBufferClass* self,
                                                 ColorBufferClass* colorBuffer)
{
  int		i;
  float		r;
  float		g;
  float		b;

  i = 0;
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

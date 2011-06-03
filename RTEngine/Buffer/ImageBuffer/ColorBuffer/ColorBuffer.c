#include <string.h>
#include <math.h>

#include "RTTI.h"
#include "RTEngine.h"
#include "ColorBuffer.h"
#include "Utils.h"
#include "Maths.h"

static void     ColorBufferConstructor(ColorBufferClass* self, va_list* list);
static void     ColorBufferDestructor(ColorBufferClass* self);
static void	colorBufferConvertToColorBuffer(ColorBufferClass* self,
                                                ColorBufferClass* colorBuffer);
static Color	colorBufferGetColor(ColorBufferClass* self, int i);
static Color	*colorBufferGetColorPtr(ColorBufferClass* self, int i);
static void	colorBufferOffsetSet(ColorBufferClass *self,
                                     int i, Color color);
static void	colorBufferSet(ColorBufferClass* self,
                               int x, int y, Color color);
static void	colorBufferReset(ColorBufferClass *self);
static void	colorBufferSetColumnFromBuf(ColorBufferClass *self,
                                            Color *buf, int col);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static ColorBufferClass       _descr = { /* ColorBuffer */
  {/* ImageBuffer */
    { /* Class */
      sizeof(ColorBufferClass),
      __RTTI_COLORBUFFER, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
      &ColorBufferConstructor,
      &ColorBufferDestructor,
    },
    &colorBufferConvertToColorBuffer,
    0, /* id */
    0, /* width */
    0, /* height */
    {
    }
  },
  &colorBufferGetColor,
  &colorBufferGetColorPtr,
  &colorBufferOffsetSet,
  &colorBufferSet,
  &colorBufferReset,
  &colorBufferSetColumnFromBuf,
  {
    0,
  }
};

Class* ColorBuffer = (Class*) &_descr;

static void     ColorBufferConstructor(ColorBufferClass* self,
                                             va_list* list)
{
  ImageBuffer->__constructor__(self, list);
  self->private.mBuffer =
    xmalloc(sizeof(*self->private.mBuffer)
        * IMAGEBUF(self)->width
        * IMAGEBUF(self)->height);
  memset(self->private.mBuffer, 0,
      sizeof(*PRIV(mBuffer))
      * IMAGEBUF(self)->width
      * IMAGEBUF(self)->height);
}

static void     ColorBufferDestructor(ColorBufferClass* self)
{
  free(self->private.mBuffer);
}

/*
 * not used ATM
 */
static void	colorBufferConvertToColorBuffer(ColorBufferClass* self,
                                                ColorBufferClass* colorBuffer)
{
  int		i;

  i = 0;
  while (i < IMAGEBUF(self)->width * IMAGEBUF(self)->height)
  {
    colorBuffer->set(colorBuffer,
        i % IMAGEBUF(colorBuffer)->width,
        i / IMAGEBUF(colorBuffer)->width,
        self->private.mBuffer[i]);
    i++;
  }
}

static Color	colorBufferGetColor(ColorBufferClass* self, int i)
{
  return (self->private.mBuffer[i]);
}

static Color	*colorBufferGetColorPtr(ColorBufferClass* self, int i)
{
  return (&self->private.mBuffer[i]);
}

static void	colorBufferOffsetSet(ColorBufferClass *self,
                                     int i, Color color)
{
  self->private.mBuffer[i] = color;
}

static void	colorBufferSet(ColorBufferClass* self,
                               int x, int y, Color color)
{
  int		i;

  i = y * IMAGEBUF(self)->width + x;
  self->private.mBuffer[i] = color;
}

static void	colorBufferReset(ColorBufferClass *self)
{
  int		size;

  size = IMAGEBUF(self)->width * IMAGEBUF(self)->height;
  memset(self->private.mBuffer,
      0,
      sizeof(*self->private.mBuffer) * size);
}

static void	colorBufferSetColumnFromBuf(ColorBufferClass *self,
                                            Color *buf, int col)
{
  int		lin;

  lin = 0;
  while (lin < IMAGEBUF(self)->height)
  {
    self->set(self, col, lin, buf[lin]);
    lin++;
  }
}

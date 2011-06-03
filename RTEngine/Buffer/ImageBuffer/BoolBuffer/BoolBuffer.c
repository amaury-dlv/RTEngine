#include "RTTI.h"
#include "BoolBuffer.h"
#include "ColorBuffer.h"
#include "Maths.h"
#include "Utils.h"

static void     BoolBufferConstructor(BoolBufferClass *self, va_list *list);
static void     BoolBufferDestructor(BoolBufferClass *self);
static bool	boolBufferGet(BoolBufferClass *self, int i);
static void	boolBufferSet(BoolBufferClass *self, int x, int y, bool bit);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static BoolBufferClass       _descr = { /* BoolBuffer */
  {/* ImageBuffer */
    { /* Class */
      sizeof(BoolBufferClass),
      __RTTI_BOOLBUFFER,
      &BoolBufferConstructor,
      &BoolBufferDestructor,
    },
    0,
    0, /* id */
    0, /* width */
    0, /* height */
    {
    }
  },
  &boolBufferGet,
  &boolBufferSet,
  {
    0 /* mBuffer */
  }
};

Class *BoolBuffer = (Class*) &_descr;

static void     BoolBufferConstructor(BoolBufferClass *self,
                                             va_list *list)
{
  ImageBuffer->__constructor__(self, list);
  self->private.mBuffer =
    xmalloc(sizeof(*self->private.mBuffer)
        * IMAGEBUF(self)->width
        * IMAGEBUF(self)->height
	/ 8);
}

static void     BoolBufferDestructor(BoolBufferClass *self)
{
  free(self->private.mBuffer);
}

static bool	boolBufferGet(BoolBufferClass *self, int i)
{
  return (GET(i));
}

static void	boolBufferSet(BoolBufferClass *self, int x, int y, bool bit)
{
  int		i;

  i = y * IMAGEBUF(self)->width + x;
  if (bit)
    SET(i);
  else
    UNSET(i);
}

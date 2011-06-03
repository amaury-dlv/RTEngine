#include "RTTI.h"
#include "MaterialBuffer.h"
#include "ColorBuffer.h"
#include "Utils.h"

static void     MaterialBufferConstructor();
static void     MaterialBufferDestructor(MaterialBufferClass* self);
static MaterialClass	*materialBufferGetMaterial(MaterialBufferClass* self,
                                                   int i);
static void		materialBufferSet(MaterialBufferClass* self,
                                          int x, int y,
                                          MaterialClass* material);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static MaterialBufferClass       _descr = { /* MaterialBuffer */
  {/* ImageBuffer */
    { /* Class */
      sizeof(MaterialBufferClass),
      __RTTI_MATBUFFER,
      &MaterialBufferConstructor,
      &MaterialBufferDestructor,
    },
    0,
    0, /* id */
    0, /* width */
    0, /* height */
    {
    }
  },
  &materialBufferGetMaterial,
  &materialBufferSet,
  {
    0,
  }
};

Class* MaterialBuffer = (Class*) &_descr;

static void     MaterialBufferConstructor(MaterialBufferClass* self,
                                             va_list* list)
{
  ImageBuffer->__constructor__(self, list);
  self->private.mBuffer =
    xmalloc(sizeof(*self->private.mBuffer)
        * IMAGEBUF(self)->width
        * IMAGEBUF(self)->height);
}

static void     MaterialBufferDestructor(MaterialBufferClass* self)
{
  free(self->private.mBuffer);
}

static MaterialClass	*materialBufferGetMaterial(MaterialBufferClass* self,
                                                   int i)
{
  return (self->private.mBuffer[i]);
}

static void		materialBufferSet(MaterialBufferClass* self,
                                          int x, int y,
                                          MaterialClass* mat)
{
  int			i;

  i = y * IMAGEBUF(self)->width + x;
  self->private.mBuffer[i] = mat;
}

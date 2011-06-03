#include <stdio.h>

#include "RTTI.h"
#include "Utils.h"
#include "ImageBuffer.h"

static void     imageBufferConstructor(ImageBufferClass* self, va_list* list);
static void     imageBufferDestructor(ImageBufferClass* self);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static ImageBufferClass       _descr = { /* ImageBufferClass */
  { /* Class */
    sizeof(ImageBufferClass),
    0, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
    &imageBufferConstructor,
    &imageBufferDestructor,
  },
  0, /* convertToColorBuffer */
  -1, /* id */
  0, /* width */
  0, /* height*/
  {
  }
};

Class* ImageBuffer = (Class*) &_descr;

static void     imageBufferConstructor(ImageBufferClass* self,
                                             va_list* list)
{
  self->width = va_arg(*list, int);
  self->height = va_arg(*list, int);
  if (self->__base__.__RTTI__ == __RTTI_LIGHTBUFFER)
  {
    self->id = va_arg(*list, int);
  }
}

static void     imageBufferDestructor(ImageBufferClass* self)
{
}


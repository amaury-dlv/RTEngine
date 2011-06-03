#include <stdio.h>

#include "RTTI.h"
#include "GBuffer.h"

static void     GBufferConstructor(GBufferClass *self, va_list *list);
static void     GBufferDestructor();
static void	GBufferAddBuffer(GBufferClass *self, ImageBufferClass *buffer);
static ImageBufferClass* GBufferGetBuffer(GBufferClass *self, int RTTI);
static LightBufferClass* GBufferGetLightBuffer(GBufferClass *self, int id);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static GBufferClass       _descr = { /* GBufferClass */
  { /* Class */
    sizeof(GBufferClass),
    0, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
    &GBufferConstructor,
    &GBufferDestructor,
  },
  &GBufferAddBuffer,
  &GBufferGetBuffer,
  &GBufferGetLightBuffer,
  {
    0
  }
};

Class *GBuffer = (Class*) &_descr;

static void     GBufferConstructor(GBufferClass *self,
                                   va_list *list)
{
  self->private.mBuffers = new(Vector);
}

static void     GBufferDestructor(GBufferClass *self)
{
  int		i;

  i = 0;
  while (i < self->private.mBuffers->size(self->private.mBuffers))
    delete(self->private.mBuffers->mPtrs[i++]);
  delete(self->private.mBuffers);
}

static void	GBufferAddBuffer(GBufferClass *self, ImageBufferClass *buffer)
{
  self->private.mBuffers->push_back(self->private.mBuffers, buffer);
}

static ImageBufferClass*	GBufferGetBuffer(GBufferClass *self, int RTTI)
{
  int		i;
  int		nBuffers;

  i = 0;
  nBuffers = self->private.mBuffers->size(self->private.mBuffers);
  while (i < nBuffers
      && CLASS(self->private.mBuffers->mPtrs[i])->__RTTI__ != RTTI)
    i++;
  if (i == nBuffers)
    raise("No such ImageBuffer\n");
  return (self->private.mBuffers->mPtrs[i]);
}

static LightBufferClass*	GBufferGetLightBuffer(GBufferClass *self,
                                                      int id)
{
  int				i;
  int		nBuffers;

  i = 0;
  nBuffers = self->private.mBuffers->size(self->private.mBuffers);
  while (i < nBuffers
      && !(CLASS(self->private.mBuffers->mPtrs[i])->__RTTI__
        == __RTTI_LIGHTBUFFER
      && IMAGEBUF(self->private.mBuffers->mPtrs[i])->id == id))
        i++;
  if (i == nBuffers)
    raise("No such LightBuffer\n");
  return (self->private.mBuffers->mPtrs[i]);
}

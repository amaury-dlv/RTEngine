#include <stdio.h>

#include "Utils.h"
#include "Vector.h"

static void     vectorConstructor(VectorClass *self, va_list *list);
static void     vectorDestructor();
static void	vector_push_back(VectorClass *self, void *ptr);
static void	vector_reserve(VectorClass *self, int batch);
static int	vector_size(VectorClass *self);
static void	vector_pop_back(VectorClass *self);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static VectorClass       _descr =
  { /* VectorClass */
    { /* Class */
      sizeof(VectorClass),
      0, /* RTTI (not used for as long as we can...
            dynamic binding does have a cost) */
      &vectorConstructor,
      &vectorDestructor,
    },
    &vector_push_back,
    0, /* VectorNE1.vector_pop_back will be set in the constructor*/
    &vector_reserve,
    &vector_size,
    0, /* mPtrs */
    {
      0, /* mSize */
      16, /* mBatchAllocation */
    }
  };

Class *Vector = (Class*) &_descr;

static void     vectorConstructor(VectorClass *self, va_list *list)
{
  self->mPtrs = xmalloc(sizeof(void*)  *self->private.mBatchAllocation);
  self->pop_back = &vector_pop_back;
}

static void     vectorDestructor(VectorClass *self)
{
  free(self->mPtrs);
}

static void	vector_push_back(VectorClass *self, void *ptr)
{
  if (((self->private.mSize + 1) % self->private.mBatchAllocation) == 0)
    self->mPtrs = xrealloc(self->mPtrs,
        sizeof(void*)
         *((self->private.mSize + 1) / self->private.mBatchAllocation + 1)
         *self->private.mBatchAllocation);
  self->mPtrs[self->private.mSize] = ptr;
  self->private.mSize++;
}

static void	vector_reserve(VectorClass *self, int batch)
{
  self->private.mBatchAllocation = batch;
}

static int	vector_size(VectorClass *self)
{
  return (self->private.mSize);
}

static void	vector_pop_back(VectorClass *self)
{
  if (self->private.mSize > 0)
    self->private.mSize--;
}

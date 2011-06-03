#include <stdio.h>

#include "Utils.h"
#include "Dequeue.h"

static void     dequeueConstructor(DequeueClass* self, va_list* list);
static void     dequeueDestructor();
static void	dequeue_push_back(DequeueClass* self, void* ptr);
static void	dequeue_reserve(DequeueClass* self, int batch);
static int	dequeue_size(DequeueClass* self);
static void*	dequeue_front(DequeueClass* self);
static void*	dequeue_back(DequeueClass* self);
static void	dequeue_push_front(DequeueClass* self, void* ptr);
static void*	dequeue_pop_back(DequeueClass* self);
static void*	dequeue_pop_front(DequeueClass *self);
static void	dequeue_clear(DequeueClass *self);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static DequeueClass       _descr =
  { /* DequeueClass */
    { /* Class */
      sizeof(DequeueClass),
      0, /* RTTI (not used for as long as we can...
            dynamic binding does have a cost) */
      &dequeueConstructor,
      &dequeueDestructor,
    },
    &dequeue_push_back,
    0, /* push_front */
    0, /* pop_back */
    0, /* pop_front */
    &dequeue_front,
    &dequeue_back,
    0, /* clear */
    &dequeue_reserve,
    &dequeue_size,
    0, /* mPtrs */
    {
      0, /* mSize */
      64, /* mBatchAllocation */
    }
  };

Class* Dequeue = (Class*) &_descr;

static void     dequeueConstructor(DequeueClass* self, va_list* list)
{
  self->mPtrs = xmalloc(sizeof(void*) * self->private.mBatchAllocation);
  self->push_front = dequeue_push_front;
  self->pop_back = dequeue_pop_back;
  self->pop_front = dequeue_pop_front;
  self->clear = dequeue_clear;
}

static void     dequeueDestructor(DequeueClass* self)
{
  free(self->mPtrs);
}

static void	dequeue_push_back(DequeueClass* self, void* ptr)
{
  if (((self->private.mSize + 1) % self->private.mBatchAllocation) == 0)
    self->mPtrs = xrealloc(self->mPtrs,
        sizeof(void*)
        * ((self->private.mSize + 1) / self->private.mBatchAllocation + 1)
        * self->private.mBatchAllocation);
  self->mPtrs[self->private.mSize] = ptr;
  self->private.mSize++;
}

static void	dequeue_reserve(DequeueClass* self, int batch)
{
  self->private.mBatchAllocation = batch;
}

static int	dequeue_size(DequeueClass* self)
{
  return (self->private.mSize);
}

static void*	dequeue_front(DequeueClass* self)
{
  if (self->private.mSize > 0)
    return (self->mPtrs[0]);
  return (0);
}

static void*	dequeue_back(DequeueClass* self)
{
  if (self->private.mSize > 0)
    return (self->mPtrs[self->private.mSize - 1]);
  return (0);
}

static void	dequeue_push_front(DequeueClass* self, void* ptr)
{
  void*		lastPtr;
  int		i;

  lastPtr = self->mPtrs[self->private.mSize - 1];
  self->push_back(self, lastPtr);
  i = 0;
  while (i < self->private.mSize - 1)
  {
    self->mPtrs[i + 1] = self->mPtrs[i];
    i++;
  }
  self->mPtrs[0] = ptr;
}

static void*	dequeue_pop_back(DequeueClass* self)
{
  if (self->private.mSize <= 0)
    return (0);
  self->private.mSize--;
  return (self->mPtrs[self->private.mSize]);
}

static void*	dequeue_pop_front(DequeueClass *self)
{
  int		i;
  void*		oldFront;

  if (self->private.mSize <= 0)
    return (0);
  oldFront = self->mPtrs[0];
  i = 0;
  while (i < self->private.mSize - 1)
  {
    self->mPtrs[i] = self->mPtrs[i + 1];
    i++;
  }
  self->private.mSize--;
  return (oldFront);
}

static void	dequeue_clear(DequeueClass *self)
{
  self->private.mSize = 0;
}

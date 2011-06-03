#include "TracerManager.h"
#include "Raycast.h"
#include "Whitted.h"
#include "PathTracer.h"

static void     tracerManagerConstructor(TracerManagerClass *s, va_list *l);
static void     tracerManagerDestructor();

/*
 ** The static binding of the functions from the NormeExporter cannot
 ** be done at compile time : "initializer element is not constant".
 ** It has to be done in the contructor.
 */
static TracerManagerClass       _descr = { /* TracerManagerClass */
    { /* Class */
      sizeof(TracerManagerClass),
      0, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
      &tracerManagerConstructor,
      &tracerManagerDestructor,
    },
    {
      0, /* eRaycast */
      0, /* eWhitted */
      0 /* ePathTracer */
    }
  };

Class* TracerManager = (Class*) &_descr;

static TracerManagerClass* sSingleton = 0;

TracerManagerClass*	TracerManagerGetSingletonPtr(void)
{
  return (sSingleton);
}

TracerClass*	TracerManagerGetTracer(TracerType type)
{
  if (type == eRaycast)
    return (sSingleton->private.mRaycast);
  if (type == eWhitted)
    return (sSingleton->private.mWhitted);
  if (type == ePathTracer)
    return (sSingleton->private.mPathTracer);
  raise("No such tracer.\n");
  return (0);
}

static void     tracerManagerConstructor(TracerManagerClass* self,
					 va_list* list)
{
  if (sSingleton != 0)
    raise("Cannot new a singleton twice : TracerManager\n");
  sSingleton = self;
  self->private.mRaycast = new(Raycast);
  self->private.mWhitted = new(Whitted);
  self->private.mPathTracer = new(PathTracer);
}

static void     tracerManagerDestructor(TracerManagerClass *self)
{
  delete(self->private.mRaycast);
  delete(self->private.mWhitted);
  delete(self->private.mPathTracer);
}

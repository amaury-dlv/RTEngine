#include "Tracer.h"

static void	tracerConstructor(TracerClass* self, va_list *list);
static void	tracerDestructor(TracerClass* self);

static TracerClass		_descr = { /* TracerClass */
  { /* Class */
    sizeof(TracerClass),
    0, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
    &tracerConstructor,
    &tracerDestructor
  },
  { /* protected */
  }
};

Class* Tracer = (Class*) &_descr;

static void	tracerConstructor(TracerClass* self, va_list *list)
{
}

static void	tracerDestructor(TracerClass* self)
{
}

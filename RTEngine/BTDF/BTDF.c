#include "BTDF.h"

static void     BTDFConstructor(BTDFClass *self, va_list *list);
static void     BTDFDestructor(BTDFClass *self);

static BTDFClass       _descr = { /* BTDFClass */
  { /* Class */
    sizeof(BTDFClass),
    0, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
    &BTDFConstructor,
    &BTDFDestructor,
  },
  0, /* f() */
  0, /* sampleF() */
  0, /* rho() */
  0, /* tir */
  { /* protected */
  }
};

Class *BTDF = (Class*) &_descr;

static void     BTDFConstructor(BTDFClass *self, va_list *list)
{
}

static void     BTDFDestructor(BTDFClass *self)
{
}

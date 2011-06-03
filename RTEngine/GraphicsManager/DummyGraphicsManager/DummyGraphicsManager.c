#include "RTEngine.h"
#include "DummyGraphicsManager.h"

static void     dummyGraphicsManagerConstructor();
static void     dummyGraphicsManagerDestructor();
static void     dummyGraphicsManagerDisplay(ImageBufferClass *colorBuffer);
static void     dummyGraphicsManagerEnd(void);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static DummyGraphicsManagerClass       _descr = {
  { /* GraphicsManagerClass */
    { /* Class */
      sizeof(DummyGraphicsManagerClass),
      0, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
      &dummyGraphicsManagerConstructor,
      &dummyGraphicsManagerDestructor,
    },
    &dummyGraphicsManagerDisplay, /* display (virtual) */
    &dummyGraphicsManagerEnd, /* end (virtual) */
    {
      0, /* mWidth */
      0, /* mHeight */
      0 /* mColorBuffer */
    }
  }
};

Class *DummyGraphicsManager = (Class*) &_descr;

static void     dummyGraphicsManagerConstructor(
                                           DummyGraphicsManagerClass *self,
                                           va_list *list)
{
  GraphicsManager->__constructor__(self, list);
}

static void     dummyGraphicsManagerDestructor()
{
}

static void     dummyGraphicsManagerDisplay(ImageBufferClass *colorBuffer)
{
}

static void     dummyGraphicsManagerEnd(void)
{
}

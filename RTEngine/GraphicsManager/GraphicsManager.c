#include "RTEngine.h"
#include "GraphicsManager.h"

static void     GraphicsManagerConstructor();
static void     GraphicsManagerDestructor();
static void     graphicsManagerDisplay(ImageBufferClass *colorBuffer);
static void     graphicsManagerEnd(void);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static GraphicsManagerClass       _descr = { /* GraphicsManagerClass */
  { /* Class */
    sizeof(GraphicsManagerClass),
    0, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
    &GraphicsManagerConstructor,
    &GraphicsManagerDestructor,
  },
  &graphicsManagerDisplay, /* display (virtual) */
  &graphicsManagerEnd, /* end (virtual) */
  {
    0, /* mWidth */
    0, /* mHeight */
    0 /* mColorBuffer */
  }
};

Class* GraphicsManager = (Class*) &_descr;

static GraphicsManagerClass* sSingleton = 0;

static void     GraphicsManagerConstructor(GraphicsManagerClass* self,
                                           va_list* list)
{
  if (sSingleton != 0)
    raise("Cannot new a singleton twice : GraphicsManager\n");
  sSingleton = self;
  self->protected.mWidth = RTEngineGetSingletonPtr()->getSceneWidth();
  self->protected.mHeight = RTEngineGetSingletonPtr()->getSceneHeight();
}

static void     GraphicsManagerDestructor()
{
}

GraphicsManagerClass*		GraphicsManagerGetSingletonPtr(void)
{
  return (sSingleton);
}

/*
 * These are dummy functions
 * instead of creating another graphicsmanager, we make the default
 */
static void     graphicsManagerDisplay(ImageBufferClass *colorBuffer)
{
}

static void     graphicsManagerEnd(void)
{
}

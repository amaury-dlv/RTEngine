#include "RTEngine.h"
#include "RenderPass.h"
#include "SceneSerializer.h"

static void     renderPassConstructor(RenderPassClass* self, va_list* list);
static void     renderPassDestructor();
static void	renderPassApply(RenderPassClass *self);
static void	renderPassApplyLoop(RenderPassClass *self);
static void	renderPassPrint(RenderPassClass *self, int current, int total);
static void	renderPassPrintBar(char a, int na);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static RenderPassClass       _descr = { /* RenderPassClass */
  { /* Class */
    sizeof(RenderPassClass),
    0, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
    &renderPassConstructor,
    &renderPassDestructor,
  },
  0, /* init */
  &renderPassApply, /* apply */
  &renderPassApplyLoop, /* applyLoop */
  0, /* renderPassPixel (pure virtual) */
  &renderPassPrint, /* print() */
  0 /* updateCurrentState */
};

Class* RenderPass = (Class*) &_descr;

static void     renderPassConstructor(RenderPassClass* self,
                                             va_list* list)
{
  self->apply = &renderPassApply;
  self->applyLoop = &renderPassApplyLoop;
  self->print = &renderPassPrint;
}

static void     renderPassDestructor()
{
}

int		renderPassUnserializeSet(SceneSerializerClass *serializer,
                                         xmlNodePtr node,
                                         RenderPassClass *self)
{
  return (0);
}

static void	renderPassApply(RenderPassClass *self)
{
  self->init(self);
  self->applyLoop(self);
}

static void	renderPassApplyLoop(RenderPassClass *self)
{
  int		width;
  int		height;
  int		size;
  int		x;
  int		y;

  width = RTEngineGetSingletonPtr()->getSceneWidth();
  height = RTEngineGetSingletonPtr()->getSceneHeight();
  size = width * height;
  self->print(self, 0, size);
  y = 0;
  while (y < height)
  {
    x = 0;
    while (x < width)
    {
      self->applyPixel(self, x, y);
      x++;
    }
    self->print(self, x + y * width, size);
    y++;
  }
  self->updateCurrentState(self);
  GraphicsManagerGetSingletonPtr()\
    ->display(IMAGEBUF(RenderingSceneGetSingletonPtr()->getCurrentState()));
}

static void	renderPassPrintBar(char a, int na)
{
  int		i;

  i = 0;
	while (i < na)
  {
    putchar(a);
    i++;
  }
  putchar('>');
  i = 0;
  while (i < 60 - na)
  {
    putchar(' ');
    i++;
  }
}

static void	renderPassPrint(RenderPassClass *self, int current, int total)
{
  static int	done = 0;
  static double	last_nb = -1;
  double	nb;

  nb = (((double)current) / total) * 100.0;
  if (nb > last_nb)
  {
    putchar('[');
    renderPassPrintBar('=', (int)(nb * 0.6));
    printf("] %.2f%%\r", nb);
    fflush(stdout);
  }
  last_nb = nb;
  if (last_nb == 100 && done == 0)
    printf("\n");
  done = ((last_nb == 100) ? 1 : 0);
}

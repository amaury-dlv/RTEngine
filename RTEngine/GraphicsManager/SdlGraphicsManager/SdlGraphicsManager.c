# include <stdio.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <time.h>
# include <unistd.h>
# include <SDL/SDL.h>

# include "RTTI.h"
# include "Color.h"
# include "RTEngine.h"
# include "SdlGraphicsManager.h"

static void     sdlGraphicsManagerConstructor();
static void     sdlGraphicsManagerDestructor();
static void	sdlGraphicsManagerDisplay(ColorBufferClass* colorBuffer);
static void	sdlGraphicsManagerEnd(void);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static SdlGraphicsManagerClass       _descr = { /* sdlGraphicsManagerClass */
  { /* Graphics Manager */
    { /* Class */
      sizeof(SdlGraphicsManagerClass),
      0, /* RTTI */
      &sdlGraphicsManagerConstructor,
      &sdlGraphicsManagerDestructor,
    },
    VOID(sdlGraphicsManagerDisplay),
    VOID(sdlGraphicsManagerEnd),
    {
      0, /* mWidth */
      0, /* mHeight */
      0 /* mColorBuffer */
    }
  },
  {
    0 /* mScreen */
  }
};

Class* SdlGraphicsManager = (Class*) &_descr;

static void     sdlGraphicsManagerConstructor(SdlGraphicsManagerClass* self,
                                             va_list* list)
{
  GraphicsManager->__constructor__(self, list);
  if (SDL_Init(SDL_INIT_VIDEO) == -1)
    raise("Error: can't init SDL.\n");
  if ((self->private.mScreen
        = SDL_SetVideoMode(GRAPHICSMGR(self)->protected.mWidth,
          GRAPHICSMGR(self)->protected.mHeight, 32, SDL_HWSURFACE)) == NULL)
    raise("Error: can't create SDL Window.\n");
  SDL_WM_SetCaption(RT_TITLE, NULL);
}

static void    	sdlGraphicsManagerDestructor()
{
  SDL_Quit();
}

static void	sdlGraphicsManagerDisplay(ColorBufferClass* colorBuffer)
{
  SdlGraphicsManagerClass	*this;
  unsigned int			*imagePtr;
  int				i;
  Color				color;

  i = 0;
  this = VOID(GraphicsManagerGetSingletonPtr());
  imagePtr = (unsigned int *)this->private.mScreen->pixels;
  while (i < GRAPHICSMGR(this)->protected.mHeight
      * GRAPHICSMGR(this)->protected.mWidth)
  {
    color = colorBuffer->getColor(colorBuffer, i);
    imagePtr[i] = colorGetUInt(&color);
    ++i;
  }
  SDL_Flip(this->private.mScreen);
}

static void			sdlGraphicsManagerEnd(void)
{
  SDL_Event			event;

  while (1)
  {
    if (SDL_WaitEvent(&event) == 0)
      raise("Error: can't handle event.\n");
    if (event.type)
      if ((event.type == SDL_KEYDOWN && event.key.keysym.sym == 27)
          || (event.type == SDL_QUIT))
        exit(EXIT_SUCCESS);
  }
}

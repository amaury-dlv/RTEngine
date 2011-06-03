#ifndef  SDLGRAPHICMANAGER_H_
# define SDLGRAPHICMANAGER_H_

# ifndef NO_GRAPHICS
#  include <SDL/SDL.h>

#  include "OOC.h"
#  include "GraphicsManager.h"
#  include "Color.h"

typedef struct SdlGraphicsManagerClass	SdlGraphicsManagerClass;
struct                          	SdlGraphicsManagerClass
{
  /* SuperClass : */
  GraphicsManagerClass			__base__;
  /* public : */
  /* private : */
  struct
  {
    SDL_Surface				*mScreen;
  } private;
};

extern Class*				SdlGraphicsManager;

# endif

#endif /* !SDLGRAPHICMANAGER_H_ */

#ifndef __GRAPHICMANAGER_H__
# define __GRAPHICMANAGER_H__

# include <stdio.h>

# include "OOC.h"
# include "ColorBuffer.h"

# define GRAPHICSMGR(x)			((GraphicsManagerClass*) (x))

# define RT_TITLE			"RTEngine"

typedef struct GraphicsManagerClass	GraphicsManagerClass;
struct                          	GraphicsManagerClass
{
  /* SuperClass : */
  Class				__base__;
  /* public : */
  void				(*display)(ImageBufferClass* colorBuffer);
  void				(*end)(void);
  /* private : */
  struct
  {
    int				mWidth;
    int				mHeight;
    ColorBufferClass*		mColorBuffer;
  } protected;
};

GraphicsManagerClass*		GraphicsManagerGetSingletonPtr(void);

extern Class*			GraphicsManager;

#endif /* !__GRAPHICMANAGER_H__ */

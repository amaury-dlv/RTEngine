#ifndef __MLXGRAPHICMANAGER_H__
# define __MLXGRAPHICMANAGER_H__

# ifndef NO_GRAPHICS
#  include <stdio.h>

#  include "OOC.h"
#  include "GraphicsManager.h"

typedef struct MlxGraphicsManagerClass	MlxGraphicsManagerClass;
struct                          	MlxGraphicsManagerClass
{
  /* SuperClass : */
  GraphicsManagerClass			__base__;
  /* public : */
  /* private : */
  struct
  {
    void*				mMlxPtr;
    void*				mWinPtr;
    void*				mImgPtr;
    unsigned int*			mImgDataPtr;
  } private;
};

extern Class*				MlxGraphicsManager;

# endif

#endif /* !__MLXGRAPHICMANAGER_H__ */

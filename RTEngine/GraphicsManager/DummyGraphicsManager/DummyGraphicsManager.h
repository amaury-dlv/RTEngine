#ifndef __DUMMYGRAPHICSMANAGER_H__
# define __DUMMYGRAPHICSMANAGER_H__

# include "GraphicsManager.h"

typedef struct DummyGraphicsManagerClass	DummyGraphicsManagerClass;
struct                          		DummyGraphicsManagerClass
{
  /* SuperClass : */
  GraphicsManagerClass			__base__;
};

extern Class*				DummyGraphicsManager;

#endif /* !__DUMMYGRAPHICSMANAGER_H__ */

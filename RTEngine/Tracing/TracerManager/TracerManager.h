#ifndef __TRACERMANAGER__
# define __TRACERMANAGER__

#include "OOC.h"
#include "Tracer.h"

typedef enum			TracerType
{
  eRaycast,
  eWhitted,
  ePathTracer
}				TracerType;

typedef struct TracerManagerClass	TracerManagerClass;
struct					TracerManagerClass
{
  /* SuperClass : */
  Class				__base__;
  /* public : */
  /* private : */
  struct
  {
    TracerClass			*mRaycast;
    TracerClass			*mWhitted;
    TracerClass			*mPathTracer;
  } private;
};

TracerManagerClass* 	TracerManagerGetSingletonPtr(void);
TracerClass*		TracerManagerGetTracer(TracerType type);

extern Class* 		TracerManager;

#endif /* !__TRACERMANAGER__H__ */

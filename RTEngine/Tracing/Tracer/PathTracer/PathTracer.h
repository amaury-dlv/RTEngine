#ifndef __PATHTRACER_H__
# define __PATHTRACER_H__

# include "Tracer.h"
# include "HitRecord.h"
# include "Color.h"

# define	PATHTRACER(x)	((PathTracerClass*) (x))

typedef struct PathTracerClass	PathTracerClass;
struct                          PathTracerClass
{
  /* SuperClass : */
  TracerClass			__base__;
  /* public : */
  Color				(*trace)(PathTracerClass* self, RayStruct* ray,
                                         int depth);
  /* public : */
  /* protected : */
  struct
  {
    int				mDepth;
  } private;
};

extern Class*			PathTracer;

#endif /* !__PATHTRACER_H__ */

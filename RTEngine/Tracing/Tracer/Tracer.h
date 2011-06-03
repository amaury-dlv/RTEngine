#ifndef __TRACER_H__
# define __TRACER_H__

# include "OOC.h"
# include "Scene.h"
# include "Ray.h"
# include "Color.h"

typedef struct TracerClass	TracerClass;
struct                          TracerClass
{
  /* SuperClass : */
  Class				__base__;
  /* public : */
  /* public : */
  /* protected : */
  struct
  {
  } protected;
};

extern Class*			Tracer;

#endif /* !__TRACER_H__ */

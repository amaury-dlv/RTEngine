#ifndef __WHITTED_H__
# define __WHITTED_H__

# include "Tracer.h"
# include "HitRecord.h"
# include "Color.h"

# define	WHITTED(x)	((WhittedClass*) (x))

typedef struct WhittedClass	WhittedClass;
struct                          WhittedClass
{
  /* SuperClass : */
  TracerClass			__base__;
  /* public : */
  Color				(*trace)(WhittedClass *self, RayStruct *ray, int depth);
  /* public : */
  /* protected : */
  struct
  {
    int				mDepth;
  } private;
};

extern Class*			Whitted;

#endif /* !__WHITTED_H__ */

#ifndef __MULTIJITTERED_H__
# define __MULTIJITTERED_H__

# include "Sampler.h"

# define MULTIJITTERED(x)		((MultiJitteredClass*) (x))

typedef struct MultiJitteredClass	MultiJitteredClass;
struct					MultiJitteredClass
{
  SamplerClass			__base__;

  /* public : */
  /* private : */
  struct
  {
    float			mSubcellWidth;
  } private;
};

extern Class*			MultiJittered;

#endif /* !__MULTIJITTERED_H__ */

#ifndef __PURERANDOM_H__
# define __PURERANDOM_H__

# include "Sampler.h"

typedef struct PureRandomClass	PureRandomClass;
struct				PureRandomClass
{
  SamplerClass			__base__;

  /* public : */
  /* private : */
  struct
  {
  } private;
};

extern Class*			PureRandom;

#endif /* !__PURERANDOM_H__ */

#ifndef __EMISSIVE_H__
# define __EMISSIVE_H__

# include "Material.h"

# define EMISSIVE(x)		((EmissiveClass*) (x))

typedef struct EmissiveClass	EmissiveClass;
struct                          EmissiveClass
{
  /* SuperClass : */
  MaterialClass			__base__;
  /* public : */
  /* public : */
  void				(*scaleRadiance)(EmissiveClass *self, float ls);
  Color				(*getLe)(EmissiveClass *self);
  /* private : */
  struct
  {
    float			mLs;
  } private;
};

extern Class*			Emissive;

void	*emissiveUnserialize();

#endif /* !__EMISSIVE_H__ */

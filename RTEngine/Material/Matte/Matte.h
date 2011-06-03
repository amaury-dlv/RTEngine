#ifndef __MATTE_H__
# define __MATTE_H__

# include "Material.h"

# define MATTE(x)		((MatteClass*) (x))

typedef struct MatteClass	MatteClass;
struct                          MatteClass
{
  /* SuperClass : */
  MaterialClass			__base__;
  /* public : */
  /* public : */
  void				(*setKd)(MatteClass *self, float kd);
  /* private : */
  struct
  {
    void			*mDiffuseBrdf;
  } private;
};

extern Class*			Matte;

void	*matteUnserialize();

#endif /* !__MATTE_H__ */

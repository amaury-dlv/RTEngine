#ifndef __PERFECTSPECULAR_H__
# define __PERFECTSPECULAR_H__

# include "OOC.h"
# include "BRDF.h"
# include "Color.h"
# include "Maths.h"

# define	PERFECTSPEC(x)	((PerfectSpecularClass *) (x))

typedef struct PerfectSpecularClass	PerfectSpecularClass;
struct                          	PerfectSpecularClass
{
  /* SuperClass : */
  BRDFClass			__base__;
  void				(*setKr)(PerfectSpecularClass *self, float kr);
  void				(*setCr)(PerfectSpecularClass *self, float cr);
  /* private : */
  struct
  {
    float			mKr;
    float			mCr;
  } private;
};

extern Class*			PerfectSpecular;

#endif /* !__PERFECTSPECULAR_H__ */

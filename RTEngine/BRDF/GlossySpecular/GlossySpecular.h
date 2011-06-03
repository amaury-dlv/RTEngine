#ifndef __GLOSSYSPECULAR_H__
# define __GLOSSYSPECULAR_H__

# include "OOC.h"
# include "BRDF.h"
# include "Color.h"
# include "Maths.h"

# define	GLOSSYSPEC(x)	((GlossySpecularClass *) (x))

typedef struct GlossySpecularClass	GlossySpecularClass;
struct                          	GlossySpecularClass
{
  /* SuperClass : */
  BRDFClass			__base__;
  void				(*setKs)(GlossySpecularClass *self, float kd);
  void				(*setCr)(GlossySpecularClass *self, float cr);
  void				(*setExp)(GlossySpecularClass *self, float exp);
  /* private : */
  struct
  {
    float			mKs;
    float			mCr;
    float			mExp;
  } private;
};

extern Class*			GlossySpecular;

#endif /* !__GLOSSYSPECULAR_H__ */

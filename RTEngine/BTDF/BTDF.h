#ifndef __BTDF_H__
# define __BTDF_H__

# include "OOC.h"
# include "Color.h"
# include "HitRecord.h"
# include "Vector3D.h"
# include "Point3D.h"

# define	BTDFT(x)	((BTDFClass *)(x))

typedef struct BTDFClass	BTDFClass;
struct                          BTDFClass
{
  /* SuperClass : */
  Class				__base__;
  /* public : */
  float				(*f)();
  Color				(*sampleF)();
  float				(*rho)();
  bool				(*tir)();
  /* protected : */
  struct
  {
  } protected;
};

extern Class*			BTDF;

#endif /* !__BTDF_H__ */

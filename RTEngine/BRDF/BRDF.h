#ifndef __BRDF_H__
# define __BRDF_H__

# include "OOC.h"
# include "Color.h"
# include "SamplerManager.h"
# include "HitRecord.h"
# include "Vector3D.h"
# include "Point3D.h"

# define	BRDFT(x)	((BRDFClass *)(x))

typedef struct BRDFClass	BRDFClass;
struct                          BRDFClass
{
  /* SuperClass : */
  Class				__base__;
  /* public : */
  float				(*f)();
  Color				(*sampleF)();
  Color				(*sampleFPath)();
  float				(*rho)();
  void				(*setSamplerType)(BRDFClass *self,
                                                  SamplerType samplerType);
  /* protected : */
  struct
  {
    SamplerClass		*mSampler;
  } protected;
};

extern Class*			BRDF;

#endif /* !__BRDF_H__ */

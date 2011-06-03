#ifndef __FISHEYE_H__
# define __FISHEYE_H__

# include "Camera.h"

# define FISHEYE(x)		((FishEyeClass*) (x))

typedef struct FishEyeClass	FishEyeClass;
struct                          FishEyeClass
{
  /* SuperClass : */
  CameraClass			__base__;
  /* public : */
  /* private : */
  struct
  {
    float			mPsi;
    float			mRSquared;
  } private;
};

extern Class*			FishEye;

void		*fishEyeUnserialize();

#endif /* !__FISHEYE_H__ */

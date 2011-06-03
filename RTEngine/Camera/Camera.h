#ifndef __CAMERA_H__
# define __CAMERA_H__

# include "OOC.h"
# include "Ray.h"
# include "Vector3D.h"
# include "Point3D.h"

# define CAMERA(x)		((CameraClass*) (x))

typedef struct CameraClass	CameraClass;
struct                          CameraClass
{
  /* SuperClass : */
  Class				__base__;
  /* public : */
  void				(*setEye)(CameraClass* self,
                                          double x, double y, double z);
  void				(*setLookAt)(CameraClass* self,
                                             double x, double y, double z);
  void				(*setUp)(CameraClass* self,
                                         double x, double y, double z);
  RayStruct			(*getRay)(CameraClass* self, float x, float y);
  void				(*computeUVW)(CameraClass* self);
  /* private : */
  struct
  {
    Point3D		mEye;
    Point3D		mLookAt;
    Vector3D		mUp;
    Vector3D		mU;
    Vector3D		mV;
    Vector3D		mW;
  } protected;
};

extern Class*			Camera;

int	cameraUnserializeSet();

#endif /* !__CAMERA_H__ */

#ifndef __SCENE_H__
# define __SCENE_H__

# include <stdio.h>

# include "OOC.h"
# include "Vector.h"
# include "GeometricObject.h"
# include "Light.h"
# include "Ambient.h"
# include "EnvironmentLight.h"
# include "Camera.h"
# include "Color.h"

typedef struct SceneClass	SceneClass;
struct                          SceneClass
{
  /* SuperClass : */
  Class				__base__;
  /* public : */
  void				(*addObject)(SceneClass* self,
                                             GeometricObjectClass* obj);
  void				(*addLight)(SceneClass* self,
                                            LightClass *light);
  void				(*addAreaLight)(SceneClass* self,
                                                LightClass *light);
  void				(*setBatchObjAllocation)(SceneClass* self,
                                                         int batchAllocation);
  void				(*setBatchLightAllocation)(SceneClass* self,
                                                          int batchAllocation);
  void				(*setCamera)(SceneClass* self,
                                             CameraClass* camera);
  CameraClass*			(*getCamera)(SceneClass* self);
  void				(*setAmbient)(SceneClass* self,
                                              AmbientClass *light);
  AmbientClass			*(*getAmbientPtr)(SceneClass *self);
  void				(*setEnvLight)(SceneClass* self,
                                               EnvironmentLightClass *light);
  EnvironmentLightClass		*(*getEnvLightPtr)(SceneClass *self);
  /* public */
  CameraClass*			mCamera;
  VectorClass*			mObjects;
  VectorClass*			mLights;
  VectorClass*			mAreaLights;
  AmbientClass			*mAmbient;
  EnvironmentLightClass		*mEnvLight;
  Color				mBGColor;
};

extern Class*			Scene;

#endif /* !__SCENE_H__ */

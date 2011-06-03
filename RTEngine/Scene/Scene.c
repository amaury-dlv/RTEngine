#include <stdio.h>

#include "Utils.h"
#include "Scene.h"
#include "Light.h"
#include "Emissive.h"
#include "AreaLight.h"

static void     sceneConstructor(SceneClass *self, va_list *list);
static void     sceneDestructor();
static void	sceneAddObject(SceneClass *self, GeometricObjectClass *obj);
static void	sceneSetBatchObjAllocation(SceneClass *self, int batchAllocation);
static void	sceneSetBatchLightAllocation(SceneClass *self, int batchAllocation);
static void	sceneSetCamera(SceneClass *self, CameraClass *camera);
static CameraClass*	sceneGetCamera(SceneClass *self);
static void	sceneAddLight(SceneClass *self, LightClass *light);
static void	sceneAddAreaLight(SceneClass *self, LightClass *light);
static void	sceneSetAmbient(SceneClass *self, AmbientClass *light);
static AmbientClass	*sceneGetAmbientPtr(SceneClass *self);
static void	sceneSetEnvLight(SceneClass *self, EnvironmentLightClass *light);
static EnvironmentLightClass	*sceneGetEnvLightPtr(SceneClass *self);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static SceneClass       _descr = { /* SceneClass */
    { /* Class */
      sizeof(SceneClass),
      0, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
      &sceneConstructor,
      &sceneDestructor,
    },
    &sceneAddObject,
    &sceneAddLight,
    &sceneAddAreaLight,
    &sceneSetBatchObjAllocation,
    &sceneSetBatchLightAllocation,
    &sceneSetCamera,
    &sceneGetCamera,
    &sceneSetAmbient,
    &sceneGetAmbientPtr,
    &sceneSetEnvLight,
    &sceneGetEnvLightPtr,
    0, /* mCamera */
    0, /* mObjects */
    0, /* mLights */
    0, /* mAreaLights */
    0, /* mAmbient */
    0, /* mEnvLight */
    { /* mBGColor */
      .5,
      .5,
      .5
    }
  };

Class *Scene = (Class*) &_descr;

static void     sceneConstructor(SceneClass *self, va_list *list)
{
  self->mObjects = new(Vector);
  self->mLights = new(Vector);
  self->mAreaLights = new(Vector);
  self->mAmbient = new(Ambient);
}

static void     sceneDestructor(SceneClass *self)
{
  int		i;

  i = 0;
  while (i < self->mObjects->size(self->mObjects))
    delete(self->mObjects->mPtrs[i++]);
  delete(self->mObjects);
  i = 0;
  while (i < self->mLights->size(self->mLights))
    delete(self->mLights->mPtrs[i++]);
  delete(self->mLights);
  i = 0;
  while (i < self->mAreaLights->size(self->mAreaLights))
    delete(self->mAreaLights->mPtrs[i++]);
  delete(self->mAreaLights);
}

static void	sceneSetBatchObjAllocation(SceneClass *self, int batchAllocation)
{
  self->mObjects->reserve(self->mObjects, batchAllocation);
}

static void	sceneSetBatchLightAllocation(SceneClass *self, int batchAllocation)
{
  self->mLights->reserve(self->mLights,
                         batchAllocation);
}

static void	sceneAddObject(SceneClass *self, GeometricObjectClass *obj)
{
  LightClass	*light;

  if (obj->material && RTTI(obj->material) == RTTI(Emissive))
  {
    light = new(AreaLight, obj, obj->material);
    self->addAreaLight(self, light);
  }
  self->mObjects->push_back(self->mObjects, obj);
}

static void	sceneSetCamera(SceneClass *self, CameraClass *camera)
{
  self->mCamera = camera;
}

static CameraClass*	sceneGetCamera(SceneClass *self)
{
  return (self->mCamera);
}

static void	sceneAddLight(SceneClass *self, LightClass *light)
{
  if (RTTI(light) == __RTTI_AMBIENT)
    self->setAmbient(self, AMBIENT(light));
  else if (RTTI(light) == __RTTI_ENVIRONMENTLIGHT)
    self->setEnvLight(self, ENVLIGHT(light));
  else
    self->mLights->push_back(self->mLights, light);
}

static void	sceneAddAreaLight(SceneClass *self, LightClass *light)
{
  self->mAreaLights->push_back(self->mAreaLights, light);
}

static void	sceneSetEnvLight(SceneClass *self, EnvironmentLightClass *light)
{
  if (self->mEnvLight)
    delete(self->mEnvLight);
  self->mEnvLight = light;
}

static void	sceneSetAmbient(SceneClass *self, AmbientClass *light)
{
  if (self->mAmbient)
    delete(self->mAmbient);
  self->mAmbient = light;
}

static AmbientClass	*sceneGetAmbientPtr(SceneClass *self)
{
  return (self->mAmbient);
}

static EnvironmentLightClass	*sceneGetEnvLightPtr(SceneClass *self)
{
  return (self->mEnvLight);
}

#include <math.h>

#include "FakeSphericalLight.h"
#include "HitRecord.h"
#include "RenderingScene.h"
#include "Maths.h"
#include "SceneSerializer.h"

static void     fakeSphericalLightConstructor();
static void     fakeSphericalLightDestructor(FakeSphericalLightClass*self);
static Color	fakeSphericalLightL(FakeSphericalLightClass *self,
                                    HitRecordStruct *hitRecord);
static Vector3D	fakeSphericalLightGetDirection(FakeSphericalLightClass *self,
                                               HitRecordStruct *hitRecord);
static void	fakeSphericalLightSetPosition(FakeSphericalLightClass *self,
                                              double x, double y, double z);
static bool	fakeSphericalLightInShadow(FakeSphericalLightClass *self,
                                           RayStruct *shadowRay);

static FakeSphericalLightClass       _descr = { /* FakeSphericalLightClass */
    { /* LightClass */
      { /* Class */
        sizeof(FakeSphericalLightClass),
        __RTTI_POINTLIGHT, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
        &fakeSphericalLightConstructor,
        &fakeSphericalLightDestructor,
      },
      &fakeSphericalLightGetDirection, /* getDirection() */
      &fakeSphericalLightL, /* l() */
      VOID(&fakeSphericalLightInShadow), /* inShadow() */
      0, /* lightSetHitRecordDist */
      0, /* id */
      1., /* ls */
      {1, 1, 1}, /* color */
    },
    &fakeSphericalLightSetPosition,
    {0, 0, 0} /* mPos */
  };

Class* FakeSphericalLight = (Class*) &_descr;

static void     fakeSphericalLightConstructor(FakeSphericalLightClass *self,
                                              va_list *list)
{
  Light->__constructor__(self, list);
}

static void     fakeSphericalLightDestructor(FakeSphericalLightClass *self)
{
  Light->__destructor__(self);
}

void	*fakeSphericalLightUnserialize(SceneSerializerClass *serializer,
                                       xmlNodePtr node)
{
  FakeSphericalLightClass	*self;
  char		*strPos;

  self = new(FakeSphericalLight);
  if (lightUnserializeSet(serializer, node, self) != 0)
    return (0);
  serializer->setStrsFromProp(serializer, node, "pos", &strPos, NULL);
  if (vectSetFromStr(&self->mPos, strPos) != 0)
  {
    fprintf(stderr, "Error: bad attributes\n");
    return (0);
  }
  return (self);
}

static void	fakeSphericalLightSetPosition(FakeSphericalLightClass *self,
                                              double x, double y, double z)
{
  self->mPos.x = x;
  self->mPos.y = y;
  self->mPos.z = z;
}

static Color	fakeSphericalLightL(FakeSphericalLightClass *self,
                                    HitRecordStruct *hitRecord)
{
  LightClass	*this;

  this = LIGHT(self);
  return ((Color){this->ls * this->color.r,
                  this->ls * this->color.g,
                  this->ls * this->color.b});
}

static Vector3D	fakeSphericalLightGetDirection(FakeSphericalLightClass *self,
                                               HitRecordStruct *hitRecord)
{
  float			r = 3.0;
  Point3D		newLocation;

  newLocation.x = self->mPos.x + r * (2.0 * rand_float() - 1.0);
  newLocation.y = self->mPos.y + r * (2.0 * rand_float() - 1.0);
  newLocation.z = self->mPos.z + r * (2.0 * rand_float() - 1.0);
  newLocation.x -= hitRecord->hitPoint.x;
  newLocation.y -= hitRecord->hitPoint.y;
  newLocation.z -= hitRecord->hitPoint.z;
  vectNormalize(&newLocation);
  return (newLocation);
}

static bool	fakeSphericalLightInShadow(FakeSphericalLightClass *self,
                                           RayStruct *shadowRay)
{
  SceneClass		*scene;
  GeometricObjectClass	*obj;
  RayStruct		ray;
  double		d;
  double		t;
  int			i;

  ray = *shadowRay;
  d = vectDist(&(ray.origin), &(self->mPos));
  i = 0;
  t = Infinity;
  scene = RenderingSceneGetSingletonPtr()->getCurrentScene();
  while (i < scene->mObjects->size(scene->mObjects))
  {
    obj = scene->mObjects->mPtrs[i];
    if (obj->hit(obj, &ray, &t) && t < (d - kEpsilon) && t > ShadowKEpsilon)
    {
      return (true);
    }
    i++;
  }
  return (false);
}

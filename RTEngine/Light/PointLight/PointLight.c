#include <math.h>

#include "PointLight.h"
#include "HitRecord.h"
#include "RenderingScene.h"
#include "Maths.h"
#include "ShadowRenderPass.h"
#include "SceneSerializer.h"

static void     pointLightConstructor(PointLightClass* self, va_list* list);
static void     pointLightDestructor(PointLightClass*self);
static Color	pointLightL(PointLightClass *self, HitRecordStruct *hitRecord);
static Vector3D	pointLightGetDirection(PointLightClass *self,
                                       HitRecordStruct *hitRecord);
static void	pointLightSetPosition(PointLightClass *self, double x,
                                      double y, double z);
static bool	pointLightInShadow(PointLightClass *self,
                                   RayStruct *shadowRay);

static PointLightClass       _descr = { /* PointLightClass */
    { /* LightClass */
      { /* Class */
        sizeof(PointLightClass),
        __RTTI_POINTLIGHT, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
        &pointLightConstructor,
        &pointLightDestructor,
      },
      &pointLightGetDirection, /* getDirection() */
      &pointLightL, /* l() */
      VOID(&pointLightInShadow), /* inShadow() */
      0, /* lightSetHitRecordDist */
      0, /* id */
      1., /* ls */
      {1, 1, 1}, /* color */
    },
    &pointLightSetPosition,
    {0, 0, 0} /* mPos */
  };

Class* PointLight = (Class*) &_descr;

static void     pointLightConstructor(PointLightClass *self, va_list *list)
{
  Light->__constructor__(self, list);
}

static void     pointLightDestructor(PointLightClass *self)
{
  Light->__destructor__(self);
}

void		*pointLightUnserialize(SceneSerializerClass *serializer,
                                       xmlNodePtr node)
{
  PointLightClass	*self;
  char		*strPos;

  self = new(PointLight);
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

static void	pointLightSetPosition(PointLightClass *self, double x,
                                      double y, double z)
{
  self->mPos.x = x;
  self->mPos.y = y;
  self->mPos.z = z;
}

static Color	pointLightL(PointLightClass *self, HitRecordStruct *hitRecord)
{
  LightClass	*this;

  this = LIGHT(self);
  return ((Color){this->ls * this->color.r,
                  this->ls * this->color.g,
                  this->ls * this->color.b});
}

static Vector3D		pointLightGetDirection(PointLightClass *self,
                                               HitRecordStruct *hitRecord)
{
  Vector3D		ans;
  float			invNorme;

  ans = (Vector3D) {
    self->mPos.x - hitRecord->hitPoint.x,
    self->mPos.y - hitRecord->hitPoint.y,
    self->mPos.z - hitRecord->hitPoint.z
  };
  invNorme = 1. / sqrtf(ans.x * ans.x + ans.y * ans.y + ans.z * ans.z);
  ans = (Vector3D) {
    ans.x * invNorme,
    ans.y * invNorme,
    ans.z * invNorme
  };
  return (ans);
}

static bool		pointLightInShadow(PointLightClass *self,
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
      g_last_t = t;
      g_last_d = d;
      return (true);
    }
    i++;
  }
  return (false);
}

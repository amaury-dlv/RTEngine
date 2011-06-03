#include "Directional.h"
#include "RenderingScene.h"
#include "HitRecord.h"
#include "Maths.h"
#include "SceneSerializer.h"

static void     directionalConstructor(DirectionalClass *self, va_list *list);
static void     directionalDestructor(DirectionalClass *self);
static Color	directionalL(DirectionalClass *self,
                             HitRecordStruct *hitRecord);
static Vector3D	directionalGetDirection(DirectionalClass *self,
                                        HitRecordStruct *hitRecord);
static void	directionalSetDirection(DirectionalClass *self, double x,
                                        double y, double z);
static bool	directionalLightInShadow(DirectionalClass *self,
                                         RayStruct *shadowRay);

static DirectionalClass       _descr = { /* DirectionalClass */
    { /* LightClass */
      { /* Class */
        sizeof(DirectionalClass),
        0, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
        &directionalConstructor,
        &directionalDestructor,
      },
      &directionalGetDirection, /* getDirection() */
      &directionalL, /* l() */
      VOID(&directionalLightInShadow), /* inShadow() */
      0, /* lightSetHitRecordDist */
      0, /* id */
      1., /* ls */
      {1, 1, 1}, /* color */
    },
    &directionalSetDirection, /* setDirection() */
    { /* private */
      {0, 0, 0} /* mDir */
    }
  };

Class *Directional = (Class*) &_descr;

static void     directionalConstructor(DirectionalClass *self, va_list *list)
{
  Light->__constructor__(self, list);
}

static void     directionalDestructor(DirectionalClass *self)
{
  Light->__destructor__(self);
}

void		*directionalUnserialize(SceneSerializerClass *serializer,
                                        xmlNodePtr node)
{
  DirectionalClass 	*self;
  char			*strDir;

  self = new(Directional);
  if (lightUnserializeSet(serializer, node, self) != 0)
    return (0);
  serializer->setStrsFromProp(serializer, node, "vec", &strDir, NULL);
  if (vectSetFromStr(&(self->private.mDir), strDir) != 0)
  {
    fprintf(stderr, "Error: bad attributes\n");
    return (0);
  }
  vectNormalize(&(self->private.mDir));
  return (self);
}

static void	directionalSetDirection(DirectionalClass *self, double x,
                                        double y, double z)
{
  self->private.mDir.x = x;
  self->private.mDir.y = y;
  self->private.mDir.z = z;
  vectNormalize(&(self->private.mDir));
}

static Color	directionalL(DirectionalClass *self,
                             HitRecordStruct *hitRecord)
{
  LightClass	*this;

  this = LIGHT(self);
  return ((Color){this->ls * this->color.r,
                  this->ls * this->color.g,
                  this->ls * this->color.b});
}

static Vector3D	directionalGetDirection(DirectionalClass *self,
                                        HitRecordStruct *hitRecord)
{
  return (self->private.mDir);
}

static bool		directionalLightInShadow(DirectionalClass *self,
                                                 RayStruct *shadowRay)
{
  SceneClass		*scene;
  GeometricObjectClass	*obj;
  RayStruct		ray;
  double		t;
  double		tMin;
  int			i;

  ray = *shadowRay;
  i = 0;
  t = tMin = Infinity;
  scene = RenderingSceneGetSingletonPtr()->getCurrentScene();
  while (i < scene->mObjects->size(scene->mObjects))
  {
    obj = scene->mObjects->mPtrs[i];
    if (obj->hit(obj, &ray, &t) && t < tMin && t > ShadowKEpsilon)
      return (true);
    i++;
  }
  return (false);
}

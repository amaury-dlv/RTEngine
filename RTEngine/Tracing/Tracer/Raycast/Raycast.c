#include "Raycast.h"
#include "Maths.h"
#include "RenderingScene.h"
#include "RTTI.h"
#include "Grid.h"

static void	raycastConstructor(RaycastClass *self, va_list *list);
static void	raycastDestructor(RaycastClass *self);
static HitRecordStruct*	raycastHit(RaycastClass *self, RayStruct *ray,
                                   HitRecordStruct *hitRecord);

static RaycastClass		_descr = { /* RaycastClass */
  { /* Tracer */
    { /* Class */
      sizeof(RaycastClass),
      0,
      &raycastConstructor,
      &raycastDestructor
    },
    { /* protected */
    }
  },
  &raycastHit
};

Class *Raycast = (Class*) &_descr;

static void	raycastConstructor(RaycastClass *self, va_list *list)
{
}

static void	raycastDestructor(RaycastClass *self)
{
}

static HitRecordStruct*	raycastHit(RaycastClass *self, RayStruct *ray,
                                   HitRecordStruct *hitRecord)
{
  GeometricObjectClass*	obj;
  GeometricObjectClass*	closestObj;
  SceneClass*		scene;
  double		t;
  double		tMin;
  int			i;

  i = 0;
  t = Infinity;
  tMin = Infinity;
  hitRecordReset(hitRecord);
  hitRecord->ray = *ray;
  scene = RenderingSceneGetSingletonPtr()->getCurrentScene();
  while (i < scene->mObjects->size(scene->mObjects))
  {
    obj = scene->mObjects->mPtrs[i++];
    if (obj->hit(obj, ray, &t) && t < (tMin - kEpsilon) && (closestObj = obj)
        && (hitRecord->hit = true))
      tMin = t;
  }
  t = Infinity;
  if (hitRecord->hit == true)
    closestObj->hitWithRecord(closestObj, ray, &t, hitRecord);
  return (hitRecord);
}

#include "RTEngine.h"
#include "Whitted.h"
#include "TracerManager.h"
#include "Maths.h"
#include "RenderingScene.h"
#include "Raycast.h"

static void	whittedConstructor(WhittedClass *self, va_list *list);
static void	whittedDestructor(WhittedClass *self);
static Color	whittedTrace(WhittedClass *self, RayStruct *ray, int depth);

static WhittedClass		_descr = { /* WhittedClass */
  { /* Tracer */
    { /* Class */
      sizeof(WhittedClass),
      0, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
      &whittedConstructor,
      &whittedDestructor
    },
    { /* protected */
    }
  },
  &whittedTrace,
  { /* private */
    0 /* mDepth */
  }
};

Class *Whitted = (Class*) &_descr;

static void	whittedConstructor(WhittedClass *self, va_list *list)
{
  self->private.mDepth = RTEngineGetSingletonPtr()->getSceneDepth();
}

static void	whittedDestructor(WhittedClass *self)
{
}

static Color		whittedTrace(WhittedClass *self, RayStruct *ray, int depth)
{
  HitRecordStruct	rec;
  RaycastClass		*raycast;

  if (depth > self->private.mDepth)
    return (Color) {0, 0, 0};
  raycast = RAYCAST(TracerManagerGetTracer(eRaycast));
  raycast->hit(raycast, ray, &rec);
  if (rec.hit)
  {
    rec.reflectDepth = depth + 1;
    return (rec.material->shadeRGB(rec.material, &rec));
  }
  return ((Color) {0, 0, 0});
}

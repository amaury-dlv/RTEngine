#include "RTEngine.h"
#include "PathTracer.h"
#include "TracerManager.h"
#include "Maths.h"
#include "RenderingScene.h"
#include "Raycast.h"

static void	pathTracerConstructor(PathTracerClass* self, va_list *list);
static void	pathTracerDestructor(PathTracerClass* self);
static Color	pathTracerTrace(PathTracerClass* self, RayStruct* ray, int depth);

static PathTracerClass		_descr = { /* PathTracerClass */
  { /* Tracer */
    { /* Class */
      sizeof(PathTracerClass),
      0, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
      &pathTracerConstructor,
      &pathTracerDestructor
    },
    { /* protected */
    }
  },
  VOID(&pathTracerTrace),
  { /* private */
    0 /* mDepth */
  }
};

Class* PathTracer = (Class*) &_descr;

static void	pathTracerConstructor(PathTracerClass* self, va_list *list)
{
  self->private.mDepth = RTEngineGetSingletonPtr()->getSceneDepth();
}

static void	pathTracerDestructor(PathTracerClass* self)
{
}

static Color		pathTracerTrace(PathTracerClass *self, RayStruct *ray,
                                        int depth)
{
  HitRecordStruct	rec;
  RaycastClass		*raycast;
  LightClass		*envlight;

  if (depth > self->private.mDepth)
    return (Color) {0, 0, 0};
  raycast = RAYCAST(TracerManagerGetTracer(eRaycast));
  raycast->hit(raycast, ray, &rec);
  if (rec.hit)
  {
    rec.reflectDepth = depth + 1;
    return (rec.material->shadePath(rec.material, &rec));
  }
  envlight = LIGHT(RenderingSceneGetSingletonPtr()->getCurrentScene()\
    ->getEnvLightPtr(RenderingSceneGetSingletonPtr()->getCurrentScene()));
  if (envlight)
    return (envlight->l(envlight, &rec));
  return ((Color){0, 0, 0});
}

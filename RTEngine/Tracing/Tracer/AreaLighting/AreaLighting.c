#include "RTEngine.h"
#include "AreaLighting.h"
#include "TracerManager.h"
#include "Maths.h"
#include "RenderingScene.h"
#include "Raycast.h"

static void	areaLightingConstructor(AreaLightingClass* self, va_list *list);
static void	areaLightingDestructor(AreaLightingClass* self);
static Color	areaLightingTrace(AreaLightingClass* self, RayStruct* ray);

static AreaLightingClass		_descr = { /* AreaLightingClass */
  { /* Tracer */
    { /* Class */
      sizeof(AreaLightingClass),
      0, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
      &areaLightingConstructor,
      &areaLightingDestructor
    },
    { /* protected */
    }
  },
  &areaLightingTrace,
};

Class* AreaLighting = (Class*) &_descr;

static void	areaLightingConstructor(AreaLightingClass* self, va_list *list)
{
}

static void	areaLightingDestructor(AreaLightingClass* self)
{
}

static Color		areaLightingTrace(AreaLightingClass *self, RayStruct *ray)
{
  HitRecordStruct	rec;
  RaycastClass		*raycast;
  RayStruct		r;

  r = *ray;
  raycast = RAYCAST(TracerManagerGetTracer(eRaycast));
  raycast->hit(raycast, &r, &rec);
  if (rec.hit)
    return (rec.material->shadeAreaLight(rec.material, &rec));
  return ((Color) {0, 0, 0});
}

#include "HitRecord.h"
#include "RenderingScene.h"
#include "NormalBuffer.h"
#include "RayBuffer.h"
#include "PointBuffer.h"
#include "DepthBuffer.h"
#include "BoolBuffer.h"
#include "Utils.h"

void	hitRecordReset(HitRecordStruct *self)
{
  self->color = (Color){0., 0., 0.};
  self->material = 0;
  self->hit = 0;
  self->normal = (Vector3D){1., 0., 0.};
  self->ray.direction = (Vector3D){0., 0., 0.};
  self->ray.origin = (Vector3D){0., 0., 0.};
  self->hitPoint = (Vector3D){0., 0., 0.};
  self->localHitPoint = (Vector3D){0., 0., 0.};
  self->reflectDepth = 0;
  self->inside = 0;
}

double	hitRecordGetDepth(HitRecordStruct *self)
{
  return (vectDist(&(self->hitPoint), &(self->ray.origin)));
}

HitRecordStruct	*hitRecordCopy(HitRecordStruct *hitRecord)
{
  HitRecordStruct	*rec;

  rec = xmalloc(sizeof(*rec));
  *rec = *hitRecord;
  return (rec);
}

static NormalBufferClass	*normalBuffer;
static PointBufferClass		*pointBuffer;
static PointBufferClass		*localPointBuffer;
static RayBufferClass		*rayBuffer;
static DepthBufferClass		*depthBuffer;
static BoolBufferClass		*insideBuffer;

static void	initBuffersPtrs(void)
{
  GBufferClass	*gBuffer;

  gBuffer = RenderingSceneGetSingletonPtr()->getGBuffer();
  normalBuffer = VOID(gBuffer->getBuffer(gBuffer, __RTTI_NORMALBUFFER));
  pointBuffer = VOID(gBuffer->getBuffer(gBuffer, __RTTI_POINTBUFFER));
  localPointBuffer =
    VOID(gBuffer->getBuffer(gBuffer, __RTTI_LOCALPOINTBUFFER));
  rayBuffer = VOID(gBuffer->getBuffer(gBuffer, __RTTI_RAYBUFFER));
  depthBuffer = VOID(gBuffer->getBuffer(gBuffer, __RTTI_DEPTHBUFFER));
  insideBuffer = VOID(gBuffer->getBuffer(gBuffer, __RTTI_BOOLBUFFER));
}

void		buildHitRecordFromGBufferWithLight(HitRecordStruct *hitRecord,
                                                   LightClass *light, int i)
{
  static bool	first = true;

  if (first == true)
    initBuffersPtrs();
  first = false;
  hitRecord->normal = normalBuffer->getNormal(normalBuffer, i);
  hitRecord->hitPoint = pointBuffer->getPoint(pointBuffer, i);
  hitRecord->localHitPoint = localPointBuffer->getPoint(localPointBuffer, i);
  hitRecord->ray = rayBuffer->getRay(rayBuffer, i);
  hitRecord->reflectDepth = 0;
  hitRecord->inside = insideBuffer->get(insideBuffer, i);
  if (light)
    light->setHitRecordDist(light, hitRecord);
}

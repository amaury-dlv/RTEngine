#include "Ray.h"
#include "Plane.h"
#include "HitRecord.h"
#include "Maths.h"
#include "Material.h"
#include "SceneSerializer.h"
#include "Utils.h"

static void     planeConstructor(PlaneClass *self, va_list *list);
static void     planeDestructor(PlaneClass *self);
static bool	planeHit(PlaneClass *self, RayStruct *ray, double *tmin);
static bool	planeHitWithRecord(PlaneClass *self, RayStruct *ray,
                                   double *tmin, HitRecordStruct *rec);
static void	planeSetNormal(PlaneClass *self, double x, double y, double z);
static bool	planeHitInt(PlaneClass *self, RayStruct *ray, double *tMin,
                            HitRecordStruct *rec);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static PlaneClass       _descr = { /* PlaneClass */
    { /* GeometricObjectClass */
      { /* Class */
	sizeof(PlaneClass),
	0, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
	&planeConstructor,
	&planeDestructor,
      },
      0, /* GeometricObjectNE1.getId will be set in the constructor */
      &planeHit, /* hit() pseudo virtual function (the binding is static not dynamic!) */
      &planeHitWithRecord, /* hitWithRecord() */
      0, /* hit() = 0; Pure virtual */
      0, /* hitWithRecord() = 0; Pure virtual */
      0, /* getNormal() ; virtual */
      0, /* pdf() ; virtual */
      0, /* sample() */
      0, /* mId */
      0, /* material */
      {0, 0, 0}, /* mPos */
      {
	0, /* mRot */
	0, /* mInvArea */
	0 /* mSampler */
      }
    },
    &planeSetNormal,
    {
      {0, 1, 0}, /* mNormal */
    }
  };

Class *Plane = (Class*) &_descr;

static void     planeConstructor(PlaneClass *self, va_list *list)
{
  GeometricObject->__constructor__(self, list);
}

static void     planeDestructor(PlaneClass *self)
{
  GeometricObject->__destructor__(GEOOBJECT(self));
}

void		*planeUnserialize(SceneSerializerClass *serializer,
				  xmlNodePtr node)
{
  PlaneClass	*self;
  char		*strNormal;

  self = new(Plane);
  if (geometricObjectUnserializeSet(serializer, node, self) != 0)
    return (0);
  serializer->setStrsFromProp(serializer, node, "normal", &strNormal, NULL);
  if (vectSetFromStr(&self->private.mNormal, strNormal) != 0)
  {
    fprintf(stderr, "Error: bad attributes\n");
    return (0);
  }
  vectNormalize(&self->private.mNormal);
  return (self);
}

static void	planeSetNormal(PlaneClass *self, double x, double y, double z)
{
  self->private.mNormal.x = x;
  self->private.mNormal.y = y;
  self->private.mNormal.z = z;
  vectNormalize(&(self->private.mNormal));
}

/*
 *
 *  t = (point - ray.o) * normal / (ray.d * normal);
 */
static bool	planeHitInt(PlaneClass *self, RayStruct *ray, double *tMin,
                            HitRecordStruct *rec)
{
  double	t;
  Vector3D	a, b;

  t = vectDotProduct(&(self->private.mNormal), &(ray->direction));
  vectOpEqual(&b, &self->private.mNormal);
  vectOpOver(&b, t);
  a = (Vector3D){0, 0, 0};
  vectOpMinus(&a, &ray->origin);
  t = vectDotProduct(&a, &b);
  if (t > kEpsilon)
  {
    *tMin = t;
    if (rec)
    {
      rec->normal = self->private.mNormal;
      vectNormalize(&(rec->normal));
      rec->material = GEOOBJECT(self)->material;
    }
    return (true);
  }
  return (false);
}
static bool	planeHitWithRecord(PlaneClass *self, RayStruct *ray,
                                   double *tMin, HitRecordStruct *rec)
{
  return (planeHitInt(self, ray, tMin, rec));
}

static bool	planeHit(PlaneClass *self, RayStruct *ray, double *tMin)
{
  return (planeHitInt(self, ray, tMin, NULL));
}

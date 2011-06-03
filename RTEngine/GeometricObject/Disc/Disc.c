#include "Ray.h"
#include "Disc.h"
#include "HitRecord.h"
#include "Maths.h"
#include "Material.h"
#include "SceneSerializer.h"
#include "Utils.h"

static void     discConstructor(DiscClass *self, va_list *list);
static void     discDestructor(DiscClass *self);
static bool	discHit(DiscClass *self, RayStruct *ray, double *tmin);
static bool	discHitWithRecord(DiscClass *self, RayStruct *ray,
                                   double *tmin, HitRecordStruct *rec);
static void	discSetNormal(DiscClass *self, double x, double y, double z);
static bool	discHitInt(DiscClass *self, RayStruct *ray, double *tMin,
                            HitRecordStruct *rec);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static DiscClass       _descr = { /* DiscClass */
    { /* GeometricObjectClass */
      { /* Class */
	sizeof(DiscClass),
	0, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
	&discConstructor,
	&discDestructor,
      },
      0, /* GeometricObjectNE1.getId will be set in the constructor */
      &discHit, /* hit() pseudo virtual function (the binding is static not dynamic!) */
      &discHitWithRecord, /* hitWithRecord() */
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
    &discSetNormal,
    {
      {0, 1, 0}, /* mNormal */
      0	 /* mRradius */
    }
  };

Class *Disc = (Class*) &_descr;

static void     discConstructor(DiscClass *self, va_list *list)
{
  GeometricObject->__constructor__(self, list);
}

static void     discDestructor(DiscClass *self)
{
  GeometricObject->__destructor__(GEOOBJECT(self));
}

void		*discUnserialize(SceneSerializerClass *serializer,
                                 xmlNodePtr node)
{
  DiscClass	*self;
  char		*strNormal, *strRadius;

  self = new(Disc);
  if (geometricObjectUnserializeSet(serializer, node, self) != 0)
    return (0);
  serializer->setStrsFromProp(serializer, node, "normal", &strNormal,
	                      "radius", &strRadius, NULL);
  if (vectSetFromStr(&self->private.mNormal, strNormal) != 0
      || setFloatFromStr(strRadius, &self->private.mRadius) != 0)
  {
    fprintf(stderr, "Error: bad attributes\n");
    return (0);
  }
  vectNormalize(&self->private.mNormal);
  return (self);
}

static void	discSetNormal(DiscClass *self, double x, double y, double z)
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
static bool	discHitInt(DiscClass *self, RayStruct *ray,
			   double *tMin,
			   HitRecordStruct *rec)
{
  double	t = vectDotProduct(&self->private.mNormal, &ray->direction);
  Vector3D	a, b, tmp;

  vectOpEqual(&b, &self->private.mNormal);
  vectOpOver(&b, t);
  a = (Vector3D){0, 0, 0};
  vectOpMinus(&a, &ray->origin);
  t = vectDotProduct(&a, &b);
  tmp = ray->direction;
  vectOpTimes(&tmp, t);
  vectOpPlus(&tmp, &ray->origin);
  vectOpMinus(&tmp, &GEOOBJECT(self)->mPos);
  if (t > kEpsilon
      && vectDotProduct(&tmp, &tmp) < pow(self->private.mRadius, 2))
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

static bool	discHitWithRecord(DiscClass *self, RayStruct *ray,
                                   double *tMin, HitRecordStruct *rec)
{
  return (discHitInt(self, ray, tMin, rec));
}

static bool	discHit(DiscClass *self, RayStruct *ray, double *tMin)
{
  return (discHitInt(self, ray, tMin, NULL));
}

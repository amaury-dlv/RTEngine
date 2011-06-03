#include <stdio.h>
#include <math.h>

#include "Cylinder.h"
#include "Ray.h"
#include "HitRecord.h"
#include "Maths.h"
#include "Material.h"
#include "Utils.h"
#include "SceneSerializer.h"

static void     cylinderConstructor(CylinderClass* self, va_list* list);
static void     cylinderDestructor(CylinderClass* self);
static bool	cylinderCheckRoot(CylinderClass* self, RayStruct* ray,
                                  double t, double* tMin,
                                  HitRecordStruct* rec);
static bool	cylinderHitInt(CylinderClass* self, RayStruct* ray,
			       double* tMin, HitRecordStruct* rec);
static bool	cylinderHitWithRecord(CylinderClass* self, RayStruct* ray,
				      double* tMin, HitRecordStruct *rec);
static bool	cylinderHit(CylinderClass* self, RayStruct* ray, double* tMin);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static CylinderClass       _descr = { /* CylinderClass */
    { /* GeometricObjectClass */
      { /* Class */
	sizeof(CylinderClass),
	0, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
	&cylinderConstructor,
	&cylinderDestructor,
      },
      0, /* GeometricObjectNE1.getId will be set in the constructor */
      &cylinderHit, /* hit() pseudo virtual function (the binding is static not dynamic!) */
      &cylinderHitWithRecord, /* hit() */
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
    { /* private */
      0, /* mLowY */
      0, /* mHighY */
      0 /* mRadius */
    }
  };

Class* Cylinder = (Class*) &_descr;

/*
 * this should need mHighY, mLowY, and mRadius when called.
 *
 * Sphere::ctor also should use mRadius when called
 */
static void     cylinderConstructor(CylinderClass* self, va_list* list)
{
  GeometricObject->__constructor__(self, list);
}

static void     cylinderDestructor(CylinderClass* self)
{
  GeometricObject->__destructor__(self);
}

void		*cylinderUnserialize(SceneSerializerClass *serializer,
                                     xmlNodePtr node)
{
  CylinderClass	*self;
  char		*strRadius, *strHighY, *strLowY;

  self = new(Cylinder);
  if (geometricObjectUnserializeSet(serializer, node, self) != 0)
    return (0);
  serializer->setStrsFromProp(serializer, node, "size", &strRadius,
			      "y1", &strHighY, "y2", &strLowY, NULL);
  if (setDoubleFromStr(strHighY, &self->private.mHighY) != 0
      || setDoubleFromStr(strLowY, &self->private.mLowY) != 0
      || setFloatFromStr(strRadius, &self->private.mRadius) != 0)
  {
    fprintf(stderr, "Error: bad attributes\n");
    return (0);
  }
  return (self);
}

/* vvv */

/*
 * normal: (ox + t * dx) / radius, 0, (oz + t * dz) / radius
 */
static bool	cylinderCheckRoot(CylinderClass* self, RayStruct* ray,
                                  double t, double* tMin,
                                  HitRecordStruct* rec)

{
  double		yHitTmp;

  if (t > kEpsilon)
  {
    yHitTmp = ray->origin.y + t * ray->direction.y;
    if (yHitTmp < self->private.mHighY && yHitTmp > self->private.mLowY)
    {
      *tMin = t;
      if (rec)
      {
        yHitTmp = 1. / self->private.mRadius;
        rec->normal.x = (ray->origin.x + t * ray->direction.x) * yHitTmp;
        rec->normal.y = 0.;
        rec->normal.z = (ray->origin.z + t * ray->direction.z) * yHitTmp;
        vectNormalize(&(rec->normal));
        rec->material = GEOOBJECT(self)->material;
      }
      return (true);
    }
  }
  return (false);
}

/*
 *  a = dx * dx + dz * dz
 *  b = 2.0 * (ox * dx + oz * dz)
 *  c = ox * ox + oz * oz - radius * radius
 */
static bool		cylinderHitInt(CylinderClass* self, RayStruct* ray,
                                       double* tMin, HitRecordStruct* rec)
{
  double a;
  double b;
  double c;
  double e;
  double delta;

  a = ray->direction.x * ray->direction.x
    + ray->direction.z * ray->direction.z;
  b = 2.0 * (ray->origin.x * ray->direction.x
	     + ray->origin.z * ray->direction.z);
  c = ray->origin.x * ray->origin.x
    + ray->origin.z * ray->origin.z
    - self->private.mRadius
    * self->private.mRadius;
  delta = b * b - 4. * a * c;
  if (delta < 0.)
    return (false);
  e = sqrt(delta);
  if (cylinderCheckRoot(self, ray, (-b - e) / (2. * a), tMin, rec))
    return (true);
  if (cylinderCheckRoot(self, ray, (-b + e) / (2. * a), tMin, rec))
    return (true);
  return (false);
}

/* ^^^ */

static bool	cylinderHitWithRecord(CylinderClass* self, RayStruct* ray,
				      double* tMin, HitRecordStruct *rec)
{
  return (cylinderHitInt(self, ray, tMin, rec));
}

static bool	cylinderHit(CylinderClass* self, RayStruct* ray, double* tMin)
{
  return (cylinderHitInt(self, ray, tMin, NULL));
}

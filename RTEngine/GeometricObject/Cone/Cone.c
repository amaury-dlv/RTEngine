#include <stdio.h>
#include <math.h>

#include "Cone.h"
#include "Ray.h"
#include "HitRecord.h"
#include "Maths.h"
#include "Material.h"
#include "SceneSerializer.h"
#include "Utils.h"

static void     coneConstructor(ConeClass *self, va_list *list);
static void     coneDestructor(ConeClass *self);
static bool	coneCheckRoot(ConeClass *self, RayStruct *ray,
                                  double t, double *tMin,
                                  HitRecordStruct *rec);
static bool	coneHitInt(ConeClass *self, RayStruct *ray,
			   double *tMin, HitRecordStruct *rec);
static bool	coneHitWithRecord(ConeClass *self, RayStruct *ray,
				  double *tMin, HitRecordStruct *rec);
static bool	coneHit(ConeClass *self, RayStruct *ray, double *tMin);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static ConeClass       _descr = { /* ConeClass */
    { /* GeometricObjectClass */
      { /* Class */
	sizeof(ConeClass),
	0, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
	&coneConstructor,
	&coneDestructor,
      },
      0, /* GeometricObjectNE1.getId will be set in the constructor */
      &coneHit, /* hit() pseudo virtual function (the binding is static not dynamic!) */
      &coneHitWithRecord, /* hit() */
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
      },
    },
    { /* private */
      0, /* mLowY */
      0, /* mHighY */
      0 /* mAlphaAngle */
    }
  };

Class *Cone = (Class*) &_descr;

static void     coneConstructor(ConeClass *self, va_list *list)
{
  GeometricObject->__constructor__(self, list);
}

static void     coneDestructor(ConeClass *self)
{
  GeometricObject->__destructor__(self);
}

void		*coneUnserialize(SceneSerializerClass *serializer,
                                 xmlNodePtr node)
{
  ConeClass	*self;
  char		*strAlpha, *strHighY, *strLowY;

  self = new(Cone);
  if (geometricObjectUnserializeSet(serializer, node, self) != 0)
    return (0);
  serializer->setStrsFromProp(serializer, node, "alpha", &strAlpha,
                              "y1", &strHighY, "y2", &strLowY, NULL);
  if (setDoubleFromStr(strHighY, &self->private.mHighY) != 0
      || setDoubleFromStr(strLowY, &self->private.mLowY) != 0
      || setFloatFromStr(strAlpha, &self->private.mAlphaAngle) != 0)
  {
    fprintf(stderr, "Error: bad attributes\n");
    return (0);
  }
  return (self);
}

/* vvv */

/*
 * normal: (ox + t * dx), -(oy + t * dy), (oz + t * dz)
 */
static bool	coneCheckRoot(ConeClass *self, RayStruct *ray,
			      double t, double *tMin,
			      HitRecordStruct *rec)
{
  double	yHitTmp;

  if (t > kEpsilon)
  {
    yHitTmp = ray->origin.y + t * ray->direction.y;
    if ((yHitTmp > self->private.mHighY && yHitTmp < self->private.mLowY))
    {
      *tMin = t;
      if (rec)
      {
        rec->normal.x = (ray->origin.x + t * ray->direction.x);
        rec->normal.y = (ray->origin.y + t * ray->direction.y)
	              * -PRIV(mAlphaAngle);
        rec->normal.z = (ray->origin.z + t * ray->direction.z);
        vectNormalize(&rec->normal);
        rec->material = GEOOBJECT(self)->material;
      }
      return (true);
    }
  }
  return (false);
}

/*
 * a = dx * dx - dy * dy + dz * dz;
 * b = 2 * (dx * ox - dy * oy + dz * oz);
 * c = ox * ox - oy * oy + oz * oz;
 */
static bool	coneHitInt(ConeClass *self, RayStruct *ray,
                                   double *tMin, HitRecordStruct *rec)
{
  Vector3D	v;
  double	ea;
  double	eb;
  double	delta;

  v.x = (pow(ray->direction.x, 2.0)
       - pow(ray->direction.y, 2.0) * PRIV(mAlphaAngle)
       + pow(ray->direction.z, 2.0));
  v.y = (2.0 * ((ray->origin.x * ray->direction.x)
	      - (ray->origin.y * ray->direction.y) * PRIV(mAlphaAngle)
	      + (ray->origin.z * ray->direction.z)));
  v.z = (pow(ray->origin.x, 2.0)
       - pow(ray->origin.y, 2.0) * PRIV(mAlphaAngle)
       + pow(ray->origin.z, 2.0));
  if ((delta = v.y * v.y - 4. * v.x * v.z) < 0.)
    return (false);
  delta = sqrt(delta);
  ea = (-v.y - delta) / (2. * v.x);
  eb = (-v.y + delta) / (2. * v.x);
  if (eb > kEpsilon && eb < ea)
    ea = eb;
  if (coneCheckRoot(self, ray, ea, tMin, rec))
    return (true);
  return (false);
}

/* ^^^ */

static bool	coneHitWithRecord(ConeClass *self, RayStruct *ray,
                                  double *tMin, HitRecordStruct *rec)
{
  return (coneHitInt(self, ray, tMin, rec));
}

static bool	coneHit(ConeClass *self, RayStruct *ray, double *tMin)
{
  return (coneHitInt(self, ray, tMin, NULL));
}

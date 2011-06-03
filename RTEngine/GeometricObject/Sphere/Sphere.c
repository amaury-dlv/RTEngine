#include <stdio.h>
#include <math.h>

#include "Sphere.h"
#include "Ray.h"
#include "HitRecord.h"
#include "Maths.h"
#include "Material.h"
#include "Utils.h"

static void     sphereConstructor(SphereClass *self, va_list *list);
static void     sphereDestructor(SphereClass*self);
static bool	sphereHit(SphereClass *self, RayStruct *ray, double *tMin);
static bool	sphereHitWithRecord(SphereClass *self, RayStruct *ray,
                                    double *tMin, HitRecordStruct *rec);
static bool	sphereHitInt(SphereClass *self, RayStruct *ray,
                             double *tMin, HitRecordStruct *rec);
static bool	sphereCheckRoot(SphereClass *self, RayStruct *ray,
                                double t, double *tMin,
                                HitRecordStruct *rec);
static float	spherePdf(SphereClass *self, HitRecordStruct *hitRecord);
static Vector3D	sphereGetNormal(SphereClass *self, Point3D *point);
static Vector3D	sphereSample(SphereClass *self);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static SphereClass       _descr = { /* SphereClass */
    { /* GeometricObjectClass */
      { /* Class */
	sizeof(SphereClass),
	0, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
	&sphereConstructor,
	&sphereDestructor,
      },
      0, /* GeometricObjectNE1.getId will be set in the constructor */
      &sphereHit, /* hit() pseudo virtual function (the binding is static not dynamic!) */
      &sphereHitWithRecord, /* hitWithRecord() */
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
      0, /* mRadius */
      false /* mIsConcave */
    }
  };

Class *Sphere = (Class*) &_descr;

static void     sphereConstructor(SphereClass *self, va_list *list)
{
  GeometricObject->__constructor__(self, list);
  GEOOBJECT(self)->pdf = &spherePdf;
  GEOOBJECT(self)->getNormal = &sphereGetNormal;
  GEOOBJECT(self)->sample = &sphereSample;
}

static void     sphereDestructor(SphereClass *self)
{
  GeometricObject->__destructor__(self);
}

void		*sphereUnserialize(SceneSerializerClass *serializer,
				   xmlNodePtr node)
{
  SphereClass	*self;
  char		*strRadius;
  char		*strConc;

  self = new(Sphere);
  if (geometricObjectUnserializeSet(serializer, node, self) != 0)
    return (0);
  serializer->setStrsFromProp(serializer, node, "size", &strRadius, NULL);
  strConc = (char *) xmlGetProp(node, XMLSTR("concave"));
  if (setFloatFromStr(strRadius, &self->private.mRadius) != 0
      || (strConc && setIntFromStr(strConc, &PRIV(mIsConcave))))
  {
    fprintf(stderr, "Error: bad attributes\n");
    return (0);
  }
  GEOOBJECT(self)->private.mInvArea = 1.
    / (4 * M_PI * PRIV(mRadius) * PRIV(mRadius));
  return (self);
}

/* vvv */

/*
 * w = t * ray.dir
 *
 * normal = (v + (t * ray.dir)) / rayon
 *        = (v + w) / rayon
 * hit_p = ray.o + (t * ray.dir)
 *       = ray.o + w
 *
 * (vector3d) tmp.t != (double) t
 *
 */
static bool	sphereCheckRoot(SphereClass *self, RayStruct *ray,
                                  double t, double *tMin,
                                  HitRecordStruct *rec)
{
  if (t > SphereKEpsilon)
  {
    *tMin = t;
    if (rec)
    {
      rec->normal.x = ray->origin.x + t * ray->direction.x;
      rec->normal.y = ray->origin.y + t * ray->direction.y;
      rec->normal.z = ray->origin.z + t * ray->direction.z;
      vectNormalize(&(rec->normal));
      rec->material = GEOOBJECT(self)->material;
    }
    return (true);
  }
  return (false);
}

/*
 * v = ray.o - self.center
 * a = ray.d * ray.d
 * b = 2 * v * ray.d
 * c = (v * v) - (self.radius * self.radius)
 */
static bool	sphereHitInt(SphereClass *self, RayStruct *ray,
                             double *tMin, HitRecordStruct *rec)
{
  double	v[3];
  double	delta;

  v[0] = ray->direction.x * ray->direction.x
    + ray->direction.y * ray->direction.y
    + ray->direction.z * ray->direction.z;
  v[1] = 2 * (ray->direction.x * ray->origin.x
	   + ray->direction.y * ray->origin.y
	   + ray->direction.z * ray->origin.z);
  v[2] = ray->origin.x * ray->origin.x
    + ray->origin.y * ray->origin.y
    + ray->origin.z * ray->origin.z
    - pow(self->private.mRadius, 2.);
  delta = v[1] * v[1] - 4 * v[0] * v[2];
  if (delta < 0.)
    return (false);
  else if (!tMin)
    return (true);
  delta = sqrt(delta);
  if (sphereCheckRoot(self, ray, (-v[1] - delta) / (2. * v[0]), tMin, rec))
    return (true);
  if (sphereCheckRoot(self, ray, (-v[1] + delta) / (2. * v[0]), tMin, rec))
    return (true);
  return (false);
}

/* ^^^ */

static bool	sphereHitWithRecord(SphereClass *self, RayStruct *ray,
                                    double *tMin, HitRecordStruct *rec)
{
  return (sphereHitInt(self, ray, tMin, rec));
}

static bool	sphereHit(SphereClass *self, RayStruct *ray, double *tMin)
{
  return (sphereHitInt(self, ray, tMin, NULL));
}

static float	spherePdf(SphereClass *self, HitRecordStruct *hitRecord)
{
  return (GEOOBJECT(self)->private.mInvArea);
}

static Vector3D	sphereGetNormal(SphereClass *self, Point3D *point)
{
  Vector3D	normal;

  normal = *point;
  vectOpMinus(&normal, &GEOOBJECT(self)->mPos);
  if (PRIV(mIsConcave))
    vectInv(&normal);
  return (normal);
}

static Vector3D	sphereSample(SphereClass *self)
{
  Vector3D	sample;

  sample = GEOOBJECT(self)->private.mSampler->\
    nextSphereSample(GEOOBJECT(self)->private.mSampler);
  vectOpPlus(&sample, &GEOOBJECT(self)->mPos);
  return (sample);
}

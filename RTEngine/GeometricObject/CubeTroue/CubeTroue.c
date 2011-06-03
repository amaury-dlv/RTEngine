#include <stdio.h>
#include <math.h>

#include "CubeTroue.h"
#include "Ray.h"
#include "HitRecord.h"
#include "Maths.h"
#include "Material.h"
#include "SceneSerializer.h"
#include "Utils.h"

static void     cubeTroueConstructor(CubeTroueClass* self, va_list* list);
static void     cubeTroueDestructor(CubeTroueClass* self);
static bool	cubeTroueCheckRoot(CubeTroueClass* self, RayStruct* ray,
                              double t, double* tMin,
                              HitRecordStruct* rec, CubeTroueInter *i);
static void	cubeTroueInter(CubeTroueClass* self, RayStruct* ray,
                          CubeTroueInter *i);
static bool	cubeTroueHitInt(CubeTroueClass* self, RayStruct* ray,
                           double* tMin, HitRecordStruct* rec);
static bool	cubeTroueHitWithRecord(CubeTroueClass* self, RayStruct* ray,
                                  double* tMin, HitRecordStruct* rec);
static bool	cubeTroueHit(CubeTroueClass* self, RayStruct* ray, double* tMin);
static bool	cubeTroueSortRoots(int num_real_roots,
                              double *roots,
                              double *record);
/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static CubeTroueClass       _descr = { /* CubeTroueClass */
    { /* GeometricObjectClass */
      { /* Class */
	sizeof(CubeTroueClass),
	0, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
	&cubeTroueConstructor,
	&cubeTroueDestructor,
      },
      0, /* GeometricObjectNE1.getId will be set in the constructor */
      &cubeTroueHit, /* hit() pseudo virtual function (the binding is static not dynamic!) */
      &cubeTroueHitWithRecord, /* hit() */
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
      0, /* hole */
      0, /* size */
      0	 /* alpha */
    }
  };

Class* CubeTroue = (Class*) &_descr;

static void     cubeTroueConstructor(CubeTroueClass* self, va_list* list)
{
  GeometricObject->__constructor__(self, list);
}

static void     cubeTroueDestructor(CubeTroueClass* self)
{
  GeometricObject->__destructor__(self);
}

void			*cubeTroueUnserialize(SceneSerializerClass *serializer,
					      xmlNodePtr node)
{
  CubeTroueClass	*self;
  char			*strHole;

  self = new(CubeTroue);
  if (geometricObjectUnserializeSet(serializer, node, self) != 0)
    return (0);
  serializer->setStrsFromProp(serializer, node, "hole", &strHole, NULL);
  if (setFloatFromStr(strHole, &self->private.hole) != 0)
  {
    fprintf(stderr, "Error: bad attributes\n");
    return (0);
  }
  return (self);
}

/* vvv */

static bool	cubeTroueCheckRoot(CubeTroueClass* self, RayStruct* ray,
                              double t, double* tMin,
                              HitRecordStruct* rec, CubeTroueInter *i)
{
  if (t > kEpsilon)
  {
    *tMin = t;
    if (rec)
    {
      t = *tMin;
      rec->normal.x = 4.0 * pow(ray->direction.x * t + ray->origin.x, 3)
	            - 10 * (ray->direction.x * t + ray->origin.x);
      rec->normal.y = 4.0 * pow(ray->direction.y * t + ray->origin.y, 3)
                    - 10 * (ray->direction.y * t + ray->origin.y);
      rec->normal.z = 4.0 * pow(ray->direction.z * t + ray->origin.z, 3)
                    - 10 * (ray->direction.z * t + ray->origin.z);
      vectNormalize(&rec->normal);
      rec->material = GEOOBJECT(self)->material;
    }
    return (true);
  }
  return (false);
}

static void			cubeTroueInter(CubeTroueClass* self, RayStruct* ray,
                                          CubeTroueInter *i)
{
  Vector3D	o;
  Vector3D	d;

  o = ray->origin;
  d = ray->direction;
  i->coeffs[4] = pow(d.x, 4) + pow(d.y, 4) + pow(d.z, 4);
  i->coeffs[3] = 4.0 * (pow(d.x, 3) * o.x + pow(d.y, 3) * o.y
                     + (pow(d.z, 3) * o.z));
  i->coeffs[2] = 6.0 * (pow(d.x, 2) * pow(o.x, 2) + pow(d.y, 2) *
                        pow(o.y, 2) + pow(d.z, 2) * pow(o.z, 2))
               - 5.0 * (pow(d.x, 2) + pow(d.y, 2) + pow(d.z, 2));
  i->coeffs[1] = 4.0 * (pow(o.x, 3) * d.x + pow(o.y, 3) * d.y
                      + pow(o.z, 3) * d.z)
               - 10.0 * (d.x * o.x + d.y * o.y + d.z * o.z);
  i->coeffs[0] = pow(o.x, 4) + pow(o.y, 4) + pow(o.z, 4)
    - 5.0 * (pow(o.x, 2) + pow(o.y, 2) + pow(o.z, 2)) + PRIV(hole);
}

static bool		cubeTroueHitInt(CubeTroueClass* self, RayStruct* ray,
					double* tMin, HitRecordStruct* rec)
{
  double		roots[4];
  int			num_real_roots;
  double		record;
  CubeTroueInter	i;

  record = Infinity;
  cubeTroueInter(self, ray, &i);
  num_real_roots = solve_quartic(i.coeffs, roots);
  if (num_real_roots == 0)
    return (false);
  if (!cubeTroueSortRoots(num_real_roots, roots, &record))
    return (false);
  if (cubeTroueCheckRoot(self, ray, record, tMin, rec, &i))
    return (true);
  return (false);
}

/* ^^^ */

static bool		cubeTroueHitWithRecord(CubeTroueClass* self, RayStruct* ray,
                                             double* tMin, HitRecordStruct* rec)
{
  return (cubeTroueHitInt(self, ray, tMin, rec));
}

static bool	cubeTroueHit(CubeTroueClass* self, RayStruct* ray, double* tMin)
{
  return (cubeTroueHitInt(self, ray, tMin, NULL));
}

static bool	cubeTroueSortRoots(int num_real_roots,
                              double *roots,
                              double *record)
{
  bool	       	intersected;
  int	       	i;

  intersected = 0;
  i = 0;
  while (i < num_real_roots)
    {
      if (roots[i] > kEpsilon)
	{
	  intersected = true;
	  if (roots[i] < *record)
	    *record = roots[i];
	}
      i++;
    }
  return (intersected);
}

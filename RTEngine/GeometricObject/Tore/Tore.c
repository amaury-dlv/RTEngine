#include <stdio.h>
#include <math.h>

#include "Tore.h"
#include "Ray.h"
#include "HitRecord.h"
#include "Maths.h"
#include "Material.h"
#include "SceneSerializer.h"
#include "Utils.h"

static void     toreConstructor(ToreClass* self, va_list* list);
static void     toreDestructor(ToreClass* self);
static bool	toreCheckRoot(ToreClass* self, RayStruct* ray,
                              double t, double* tMin,
                              HitRecordStruct* rec, ToreInter *i);
static void	toreInter(ToreClass* self, RayStruct* ray,
                          ToreInter *i);
static bool	toreHitInt(ToreClass* self, RayStruct* ray,
                           double* tMin, HitRecordStruct* rec);
static bool	toreHitWithRecord(ToreClass* self, RayStruct* ray,
                                  double* tMin, HitRecordStruct* rec);
static bool	toreHit(ToreClass* self, RayStruct* ray, double* tMin);
static bool	toreSortRoots(int num_real_roots,
                              double *roots,
                              double *record);
/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static ToreClass       _descr = { /* ToreClass */
    { /* GeometricObjectClass */
      { /* Class */
	sizeof(ToreClass),
	0, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
	&toreConstructor,
	&toreDestructor,
      },
      0, /* GeometricObjectNE1.getId will be set in the constructor */
      &toreHit, /* hit() pseudo virtual function (the binding is static not dynamic!) */
      &toreHitWithRecord, /* hit() */
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
      0, /* mT */
    }
  };

Class* Tore = (Class*) &_descr;

static void     toreConstructor(ToreClass* self, va_list* list)
{
  GeometricObject->__constructor__(self, list);
}

static void     toreDestructor(ToreClass* self)
{
  GeometricObject->__destructor__(self);
}

void		*toreUnserialize(SceneSerializerClass *serializer,
                                 xmlNodePtr node)
{
  ToreClass	*self;
  char		*strRadius;
  char		*strT;

  self = new(Tore);
  if (geometricObjectUnserializeSet(serializer, node, self) != 0)
    return (0);
  serializer->setStrsFromProp(serializer, node, "rad", &strRadius,
			      "t", &strT, NULL);
  if (setFloatFromStr(strRadius, &self->private.mRadius) != 0
      || setFloatFromStr(strT, &self->private.mT) != 0)
  {
    fprintf(stderr, "Error: bad attributes\n");
    return (0);
  }
  return (self);
}

/* vvv */

/*
 * param_squared = mRadius^2 + mT^2
 * sum_squared = (ox + t * dx)^2 + (oy + t * oy)^2 + (oz + t * oz)^2;
 * rec->normal->x = 4. * (ox + t * dx) * (sum_squared - param_squared);
 * rec->normal->y = 4. * (oy + t * dy) * (sum_squared - param_squared + 2. * self->private.mRadius^2);
 * rec->normal->z = 4. * (oz + t * dz) * (sum_squared - param_squared);
 */
static bool	toreCheckRoot(ToreClass* self, RayStruct* ray,
                              double t, double* tMin,
                              HitRecordStruct* rec, ToreInter *i)
{
  if (t > kEpsilon)
  {
    *tMin = t;
    if (rec)
    {
      i->tradius = (pow((ray->origin.x + t * ray->direction.x), 2)
      + pow((ray->origin.y + t * ray->direction.y), 2)
      + pow((ray->origin.z + t * ray->direction.z), 2))
      - (pow(self->private.mRadius, 2.) + pow(self->private.mT, 2.));
      rec->normal.x = 4. * (ray->origin.x + t * ray->direction.x)
        * (i->tradius);
      rec->normal.y = 4. * (ray->origin.y + t * ray->direction.y)
        * (i->tradius + 2. * pow(self->private.mRadius, 2.));
      rec->normal.z = 4. * (ray->origin.z + t * ray->direction.z)
        * (i->tradius);
      vectNormalize(&(rec->normal));
      vectOpEqual(&(i->t), &(i->w));
      vectOpPlus(&(i->t), &(ray->origin));
      rec->material = GEOOBJECT(self)->material;
    }
    return (true);
  }
  return (false);
}

static void			toreInter(ToreClass* self, RayStruct* ray,
                                          ToreInter *i)
{
  i->sum_d_sqrd = pow(ray->direction.x, 2.) + pow(ray->direction.y, 2.)
  + pow(ray->direction.z, 2.);
  i->e = pow(ray->origin.x, 2.) + pow(ray->origin.y, 2.)
  + pow(ray->origin.z, 2.) - pow(self->private.mRadius, 2.)
  - pow(self->private.mT, 2.);
  i->f = ray->origin.x * ray->direction.x
  + ray->origin.y * ray->direction.y + ray->origin.z * ray->direction.z;
  i->four_a_sqrd = 4.0 * self->private.mRadius * self->private.mRadius;
  i->coeffs[0] = pow(i->e, 2.) - i->four_a_sqrd
  * (self->private.mT * self->private.mT - ray->origin.y * ray->origin.y);
  i->coeffs[1] = 4. * i->f * i->e
  + 2. * i->four_a_sqrd * ray->origin.y * ray->direction.y;
  i->coeffs[2] = 2. * i->sum_d_sqrd * i->e + 4.0
  * pow(i->f, 2.) + i->four_a_sqrd * pow(ray->direction.y, 2.);
  i->coeffs[3] = 4. * i->sum_d_sqrd * i->f;
  i->coeffs[4] = pow(i->sum_d_sqrd, 2.);
}

static bool		toreHitInt(ToreClass* self, RayStruct* ray,
                                   double* tMin, HitRecordStruct* rec)
{
  double	roots[4];
  int		num_real_roots;
  double	record;
  ToreInter	i;

  record = Infinity;
  toreInter(self, ray, &i);
  num_real_roots = solve_quartic(i.coeffs, roots);
  if (num_real_roots == 0)
    return (false);
  if (!toreSortRoots(num_real_roots, roots, &record))
    return (false);
  if (toreCheckRoot(self, ray, record, tMin, rec, &i))
    return (true);
  return (false);
}

/* ^^^ */

static bool		toreHitWithRecord(ToreClass* self, RayStruct* ray,
                                          double* tMin, HitRecordStruct* rec)
{
  return (toreHitInt(self, ray, tMin, rec));
}

static bool	toreHit(ToreClass* self, RayStruct* ray, double* tMin)
{
  return (toreHitInt(self, ray, tMin, NULL));
}

static bool	toreSortRoots(int num_real_roots,
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

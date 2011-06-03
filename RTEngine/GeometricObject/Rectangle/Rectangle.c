#include <stdio.h>
#include <math.h>

#include "Rectangle.h"
#include "Ray.h"
#include "HitRecord.h"
#include "Maths.h"
#include "Material.h"
#include "Utils.h"

static void     rectangleConstructor(RectangleClass *self, va_list *list);
static void     rectangleDestructor(RectangleClass*self);
static bool	rectangleHit(RectangleClass *self, RayStruct *ray, double *tMin);
static bool	rectangleHitWithRecord(RectangleClass *self, RayStruct *ray,
                                    double *tMin, HitRecordStruct *rec);
static bool	rectangleHitInt(RectangleClass *self, RayStruct *ray,
                             double *tMin, HitRecordStruct *rec);
static void	rectangleGetDist(RectangleClass *self, RayStruct *ray,
				 double *t, Vector3D *tmp);
static float	rectanglePdf(RectangleClass *self, HitRecordStruct *hitRecord);
static Vector3D	rectangleGetNormal(RectangleClass *self, Point3D *point);
static Vector3D	rectangleSample(RectangleClass *self);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static RectangleClass       _descr = { /* RectangleClass */
    { /* GeometricObjectClass */
      { /* Class */
	sizeof(RectangleClass),
	0, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
	&rectangleConstructor,
	&rectangleDestructor,
      },
      0, /* GeometricObjectNE1.getId will be set in the constructor */
      &rectangleHit, /* hit() pseudo virtual function (the binding is static not dynamic!) */
      &rectangleHitWithRecord, /* hitWithRecord() */
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
      {0, 0, 1}, /* n */
      {1, 0, 0}, /* u */
      {0, 1, 0}, /* v */
      0, /* lenU */
      0 /* lenV */
    }
  };

Class *Rectangle = (Class*) &_descr;

static void     rectangleConstructor(RectangleClass *self, va_list *list)
{
  GeometricObject->__constructor__(self, list);
  GEOOBJECT(self)->pdf = &rectanglePdf;
  GEOOBJECT(self)->getNormal = &rectangleGetNormal;
  GEOOBJECT(self)->sample = &rectangleSample;
}

static void     rectangleDestructor(RectangleClass *self)
{
  GeometricObject->__destructor__(self);
}

void		rectangleUnserializeSetUV(RectangleClass *self)
{
  PRIV(lenU) = vectLength(&PRIV(u));
  PRIV(lenU) *= PRIV(lenU);
  PRIV(lenV) = vectLength(&PRIV(v));
  PRIV(lenV) *= PRIV(lenV);
}

void			*rectangleUnserialize(SceneSerializerClass *serializer,
					      xmlNodePtr node)
{
  RectangleClass	*self;
  char			*strU, *strV, *strN;

  self = new(Rectangle);
  if (geometricObjectUnserializeSet(serializer, node, self) != 0)
    return (0);
  serializer->setStrsFromProp(serializer, node, "u", &strU, "v", &strV, NULL);
  if (vectSetFromStr(&PRIV(u), strU) || vectSetFromStr(&PRIV(v), strV))
  {
    fprintf(stderr, "Error: bad attributes\n");
    return (0);
  }
  strN = (char*)xmlGetProp(node, XMLSTR("normal"));
  if (strN != 0)
  {
    if (vectSetFromStr(&self->private.n, strN))
    {
      self->private.n = self->private.u;
      vectCrossProduct(&self->private.n, &self->private.v);
    }
  }
  rectangleUnserializeSetUV(self);
  GEOOBJECT(self)->private.mInvArea = 1.
    / (vectLength(&PRIV(u)) * vectLength(&PRIV(v)));
  return (self);
}

static void		rectangleGetDist(RectangleClass *self, RayStruct *ray,
					 double *t, Vector3D *tmp)
{
  *tmp = GEOOBJECT(self)->mPos;
  *tmp = (Vector3D){0, 0, 0};
  vectOpMinus(tmp, &ray->origin);
  *t = vectDotProduct(tmp, &PRIV(n));
  *t /= vectDotProduct(&ray->direction, &PRIV(n));
}

static bool		rectangleHitInt(RectangleClass *self, RayStruct *ray,
                                     double *tMin, HitRecordStruct *rec)
{
  Vector3D tmp;
  double t;
  double ddotx;

  rectangleGetDist(self, ray, &t, &tmp);
  if (t <= RectangleKEpsilon)
    return (false);
  tmp = ray->direction;
  vectOpTimes(&tmp, t);
  vectOpPlus(&tmp, &ray->origin);
  ddotx = vectDotProduct(&tmp, &PRIV(u));
  if (ddotx < 0.0 || ddotx > PRIV(lenU))
    return (false);
  ddotx = vectDotProduct(&tmp, &PRIV(v));
  if (ddotx < 0.0 || ddotx > PRIV(lenV))
    return (false);
  *tMin = t;
  if (rec)
  {
    rec->hitPoint = tmp;
    rec->normal = PRIV(n);
    vectNormalize(&(rec->normal));
    rec->material = GEOOBJECT(self)->material;
  }
  return (true);
}

/* ^^^ */

static bool	rectangleHitWithRecord(RectangleClass *self, RayStruct *ray,
                                    double *tMin, HitRecordStruct *rec)
{
  return (rectangleHitInt(self, ray, tMin, rec));
}

static bool	rectangleHit(RectangleClass *self, RayStruct *ray, double *tMin)
{
  return (rectangleHitInt(self, ray, tMin, NULL));
}

static float	rectanglePdf(RectangleClass *self, HitRecordStruct *hitRecord)
{
  return (GEOOBJECT(self)->private.mInvArea);
}

static Vector3D	rectangleGetNormal(RectangleClass *self, Point3D *point)
{
  return (PRIV(n));
}

static Vector3D	rectangleSample(RectangleClass *self)
{
  Point2D	sample;
  Vector3D	u;
  Vector3D	v;

  sample = GEOOBJECT(self)->private.mSampler->\
           nextSample(GEOOBJECT(self)->private.mSampler);
  u = PRIV(u);
  vectOpTimes(&u, sample.x);
  v = PRIV(v);
  vectOpTimes(&v, sample.y);
  vectOpPlus(&u, &v);
  vectOpPlus(&u, &(GEOOBJECT(self)->mPos));
  return (u);
}

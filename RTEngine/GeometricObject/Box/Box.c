#include <stdio.h>
#include <math.h>

#include "Box.h"
#include "Ray.h"
#include "HitRecord.h"
#include "Maths.h"
#include "Material.h"
#include "SceneSerializer.h"
#include "Utils.h"

static void     boxConstructor(BoxClass *self, va_list *list);
static void     boxDestructor(BoxClass *self);
static void	boxHitGetNormal(int faceHit, Vector3D *normal);
static bool	boxHitWithRecord(BoxClass *self, RayStruct *ray,
                         double *tMin, HitRecordStruct *rec);
static bool	boxHit(BoxClass *self, RayStruct *ray, double *tMin);
static void	boxHitInit(BoxClass *self, RayStruct *ray, BoxInter *i);
static void	boxHitTYZ(BoxClass *self, BoxInter *i);
static void	boxHitTExVal(BoxClass *self, BoxInter *i);
static void	boxHitTEnVal(BoxClass *self, BoxInter *i);
static void	boxHitTXYZ(BoxClass *self, RayStruct *ray, BoxInter *i);

static BoxClass       _descr = { /* BoxClass */
    { /* GeometricObjectClass */
      { /* Class */
	sizeof(BoxClass),
	0, /* RTTI */
	&boxConstructor,
	&boxDestructor,
      },
      0, /* getId, binding done in GeometricObject's ctor */
      0, /* hit(), binding done in ctor */
      0, /* hitWithRecord(), likewise, binding done in ctor */
      0, /* geometricObjectHitRotate */
      0, /* geometricObjectHitWithRecordRotate */
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
      0, /* x0 */
      0, /* y0 */
      0, /* z0 */
      0, /* x1 */
      0, /* y1 */
      0 /* z1 */
    }
  };

Class *Box = (Class*) &_descr;

static void     boxConstructor(BoxClass *self, va_list *list)
{
  GeometricObject->__constructor__(self, list);
  GEOOBJECT(self)->objHit = VOID(&boxHit);
  GEOOBJECT(self)->objHitWithRecord = VOID(&boxHitWithRecord);
  PRIV(x0) = va_arg(*list, double);
  PRIV(y0) = va_arg(*list, double);
  PRIV(z0) = va_arg(*list, double);
  PRIV(x1) = va_arg(*list, double);
  PRIV(y1) = va_arg(*list, double);
  PRIV(z1) = va_arg(*list, double);
}

static void     boxDestructor(BoxClass *self)
{
  GeometricObject->__destructor__(self);
}

void		*boxUnserialize(SceneSerializerClass *serializer,
                                 xmlNodePtr node)
{
  BoxClass	*self;
  char		*a[3];
  char		*b[3];

  self = new(Box);
  if (geometricObjectUnserializeSet(serializer, node, self) != 0)
    return (0);
  serializer->setStrsFromProp(serializer, node, "ax", &a[0], "ay", &a[1], "az",
      &a[2], "bx", &b[0], "by", &b[1], "bz", &b[2], NULL);
  if (setFloatFromStr(a[0], &PRIV(x0)) != 0
      || setFloatFromStr(a[1], &PRIV(y0)) != 0
      || setFloatFromStr(a[2], &PRIV(z0)) != 0
      || setFloatFromStr(b[0], &PRIV(x1)) != 0
      || setFloatFromStr(b[1], &PRIV(y1)) != 0
      || setFloatFromStr(b[2], &PRIV(z1)) != 0)
  {
    fprintf(stderr, "Error: bad attributes\n");
    return (0);
  }
  return (self);
}

static void	boxHitGetNormal(int faceHit, Vector3D *normal)
{
  normal->x = ((faceHit == 0) ? -1 : ((faceHit == 3) ? 1 : 0));
  normal->y = ((faceHit == 1) ? -1 : ((faceHit == 4) ? 1 : 0));
  normal->z = ((faceHit == 2) ? -1 : ((faceHit == 5) ? 1 : 0));
}

static bool	boxHitWithRecord(BoxClass *self, RayStruct *ray,
                         double *tMin, HitRecordStruct *rec)
{
  BoxInter	i;

  boxHitTXYZ(self, ray, &i);
  if (i.t0 < i.t1 && i.t1 > kEpsilon)
  {
    if (i.t0 > kEpsilon)
    {
      *tMin = i.t0;
      boxHitGetNormal(i.faceIn, &rec->normal);
    }
    else
    {
      *tMin = i.t1;
      boxHitGetNormal(i.faceOut, &rec->normal);
    }
    rec->material = GEOOBJECT(self)->material;
    return (true);
  }
  return (false);
}

static bool	boxHit(BoxClass *self, RayStruct *ray, double *tMin)
{
  BoxInter	i;

  boxHitTXYZ(self, ray, &i);
  if (i.t0 < i.t1 && i.t1 > kEpsilon)
  {
    if (i.t0 > kEpsilon)
      *tMin = i.t0;
    else
      *tMin = i.t1;
    return (true);
  }
  return (false);
}

static void	boxHitInit(BoxClass *self, RayStruct *ray, BoxInter *i)
{
  i->rox = ray->origin.x;
  i->roy = ray->origin.y;
  i->roz = ray->origin.z;
  i->rdx = ray->direction.x;
  i->rdy = ray->direction.y;
  i->rdz = ray->direction.z;
  i->a = 1. / i->rdx;
  i->b = 1. / i->rdy;
  i->c = 1. / i->rdz;
  if (i->a >= 0)
  {
    i->txMin = i->a * (PRIV(x0) - i->rox);
    i->txMax = i->a * (PRIV(x1) - i->rox);
  }
  else
  {
    i->txMin = i->a * (PRIV(x1) - i->rox);
    i->txMax = i->a * (PRIV(x0) - i->rox);
  }
}

static void	boxHitTYZ(BoxClass *self, BoxInter *i)
{
  if (i->b >= 0)
  {
    i->tyMin = i->b * (PRIV(y0) - i->roy);
    i->tyMax = i->b * (PRIV(y1) - i->roy);
  }
  else
  {
    i->tyMin = i->b * (PRIV(y1) - i->roy);
    i->tyMax = i->b * (PRIV(y0) - i->roy);
  }
  if (i->c >= 0)
  {
    i->tzMin = i->c * (PRIV(z0) - i->roz);
    i->tzMax = i->c * (PRIV(z1) - i->roz);
  }
  else
  {
    i->tzMin = i->c * (PRIV(z1) - i->roz);
    i->tzMax = i->c * (PRIV(z0) - i->roz);
  }
}

static void	boxHitTExVal(BoxClass *self, BoxInter *i)
{
  if (i->txMin > i->tyMin)
  {
    i->t0 = i->txMin;
    i->faceIn = (i->a >= 0) ? 0 : 3;
  }
  else
  {
    i->t0 = i->tyMin;
    i->faceIn = (i->b >= 0) ? 1 : 4;
  }
  if (i->tzMin > i->t0)
  {
    i->t0 = i->tzMin;
    i->faceIn = (i->c >= 0) ? 2 : 5;
  }
}

static void	boxHitTEnVal(BoxClass *self, BoxInter *i)
{
  if (i->txMax < i->tyMax)
  {
    i->t1 = i->txMax;
    i->faceOut = (i->a >= 0) ? 3 : 0;
  }
  else
  {
    i->t1 = i->tyMax;
    i->faceOut = (i->b >= 0) ? 4 : 1;
  }
  if (i->tzMax < i->t1)
  {
    i->t1 = i->tzMax;
    i->faceOut = (i->c >= 0) ? 5 : 2;
  }
}

static void	boxHitTXYZ(BoxClass *self, RayStruct *ray, BoxInter *i)
{
  boxHitInit(self, ray, i);
  boxHitTYZ(self, i);
  boxHitTExVal(self, i);
  boxHitTEnVal(self, i);
}

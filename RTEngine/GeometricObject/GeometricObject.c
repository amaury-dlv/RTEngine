#include <stdio.h>
#include <math.h>

#include "GeometricObject.h"
#include "SceneSerializer.h"
#include "HitRecord.h"
#include "Point3D.h"
#include "Vector3D.h"
#include "Plane.h"
#include "Maths.h"
#include "Emissive.h"
#include "AreaLight.h"
#include "SamplerManager.h"
#include "RTEngine.h"

static void     geometricObjectConstructor();
static void     geometricObjectDestructor();
static int      geometricObjectGetId(GeometricObjectClass *self);
static void	transform(GeometricObjectClass *self, RayStruct *ray);
static bool	geometricObjectHit(GeometricObjectClass *self, RayStruct *ray,
				   double *tMin);
static bool	geometricObjectHitWithRecord(GeometricObjectClass *self,
			     RayStruct *ray,
                             double *tMin,
                             HitRecordStruct *rec);
static float	geometricObjectPdf(GeometricObjectClass *self,
                                   HitRecordStruct *hitRecord);
static Vector3D	geometricObjectGetNormal(GeometricObjectClass *self,
                                         Point3D *point);
static Vector3D	geometricObjectSample(GeometricObjectClass *self);

/* rec->normal->normalize(rec->normal); */
/* rec->normal->vectorByMatrix(rec->normal, PRIV(mRotateMatrix)); */

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static GeometricObjectClass       _descr = { /* GeometricObjectClass */
    { /* Class */
      sizeof(GeometricObjectClass),
      0, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
      &geometricObjectConstructor,
      &geometricObjectDestructor,
    },
    0, /* GeometricObjectNE1.getId will be set in the constructor */
    0, /* geometricObjectHit */
    0, /* geometricObjectHitWithRecord */
    0, /* hit() = 0; Pure virtual */
    0, /* hitWithRecord() = 0; Pure virtual */
    0, /* getNormal() ; virtual */
    0, /* pdf() ; virtual */
    0, /* sample() */
    0, /* mId */
    0, /* material */
    {0, 0, 0}, /* mPos */
    {
      0, /* mMatrix */
      0, /* mInvArea */
      0 /* mSampler */
    }
  };

Class *GeometricObject = (Class*) &_descr;

static int mCurrentId = 0;

static void     geometricObjectConstructor(GeometricObjectClass *self,
					   va_list *list)
{
  self->hit = &geometricObjectHit;
  self->hitWithRecord = &geometricObjectHitWithRecord;
  self->getNormal = &geometricObjectGetNormal;
  self->pdf = &geometricObjectPdf;
  self->sample = &geometricObjectSample;
  PRIV(mSampler) = GEOOBJECT(GeometricObject)->private.mSampler;
  if (!PRIV(mSampler))
  {
    PRIV(mSampler) = SamplerManagerGetSingletonPtr()\
      ->getSampler(eMultiJittered,
        RTEngineGetSingletonPtr()->getDefaultNumSamples(),
	RTEngineGetSingletonPtr()->getDefaultNumSets(), 100.);
    GEOOBJECT(GeometricObject)->private.mSampler = PRIV(mSampler);
    PRIV(mSampler)->mapSamplesToSphere(PRIV(mSampler));
  }
  PRIV(mInvArea) = 1;
  self->mId = mCurrentId++;
  self->getId = geometricObjectGetId;
  self->material = va_arg(*list, MaterialClass *);
  self->private.mMatrix = 0;
}

static void     geometricObjectDestructor()
{
}

void		UnserializeSetGeoObj(GeometricObjectClass *self)
{
    toRad(&(self->private.mMatrix->mRot));
    setIdentity(self->private.mMatrix);
    composeMatrix(self->private.mMatrix);
    composeInvMatrix(self->private.mMatrix);
}

int		geometricObjectUnserializeSet(SceneSerializerClass *serializer,
                                              xmlNodePtr node,
                                              GeometricObjectClass *self)
{
  char		*strIdMat, *strRot, *strPos;

  serializer->setStrsFromProp(serializer, node, "id_mat", &strIdMat, NULL);
  if (!(self->material = serializer->getMaterialFromId(serializer, strIdMat)))
  {
    fprintf(stderr, "Error: bad attributes\n");
    return (-1);
  }
  strRot = (char*)xmlGetProp(node, XMLSTR("rot"));
  if (strRot != 0)
  {
    self->private.mMatrix = malloc(sizeof(t_matrix));
    if (vectSetFromStr(&(self->private.mMatrix->mRot), strRot) != 0)
      return (0);
    UnserializeSetGeoObj(self);
  }
  strPos = (char*) xmlGetProp(node, XMLSTR("coor"));
  if (strPos != 0)
    if (vectSetFromStr(&(self->mPos), strPos) != 0)
    {
      fprintf(stderr, "Error: bad attributes\n");
      return (-1);
    }
  return (0);
}

static int      geometricObjectGetId(GeometricObjectClass *self)
{
  return (self->mId);
}

static void	transform(GeometricObjectClass *self, RayStruct *ray)
{
  translate(&ray->origin, &self->mPos);
  if (self->private.mMatrix == 0)
    return ;
  if (self->private.mMatrix != 0)
  {
    unrotate(&ray->origin, self->private.mMatrix);
    unrotate(&ray->direction, self->private.mMatrix);
  }
}

static bool	geometricObjectHit(GeometricObjectClass *self, RayStruct *ray,
				   double *tMin)
{
  RayStruct	sentRay = *ray;

  transform(self, &sentRay);
  if (self->objHit(self, &sentRay, tMin))
    return (true);
  return (false);
}

static bool	geometricObjectHitWithRecord(GeometricObjectClass *self,
			     RayStruct *ray,
                             double *tMin,
                             HitRecordStruct *rec)
{
  RayStruct	sentRay = *ray;

  transform(self, &sentRay);
  if (self->objHitWithRecord(self, &sentRay, tMin, rec))
  {
    rec->hitPoint = sentRay.direction;
    vectOpTimes(&rec->hitPoint, *tMin);
    vectOpPlus(&rec->hitPoint, &sentRay.origin);
    rec->localHitPoint = rec->hitPoint;
    if (PRIV(mMatrix) != 0)
    {
      rotate(&rec->normal, self->private.mMatrix);
      rotate(&rec->hitPoint, self->private.mMatrix);
    }
    if (vectDotProduct(&rec->normal, &ray->direction) > .0)
    {
      vectInv(&rec->normal);
      rec->inside = true;
    }
    invTranslate(&rec->hitPoint, &self->mPos);
    return (true);
  }
  return (false);
}

static float	geometricObjectPdf(GeometricObjectClass *self,
				   HitRecordStruct *hitRecord)
{
  return (PRIV(mInvArea));
}

static Vector3D	geometricObjectGetNormal(GeometricObjectClass *self,
					 Point3D *point)
{
  return ((Vector3D){0, 0, 0});
}

static Vector3D	geometricObjectSample(GeometricObjectClass *self)
{
  return (PRIV(mSampler)->nextSphereSample(PRIV(mSampler)));
}

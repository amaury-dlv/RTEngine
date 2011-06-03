#include "Reflective.h"
#include "Lambertian.h"
#include "GlossySpecular.h"
#include "Utils.h"
#include "TracerManager.h"
#include "Whitted.h"
#include "PathTracer.h"

static void	reflectiveConstructor(ReflectiveClass *self, va_list *list);
static void	reflectiveDestructor(ReflectiveClass *self);
static void	reflectiveSetCr(ReflectiveClass *self, float cr);
static void	reflectiveSetKr(ReflectiveClass *self, float kr);
static float	reflectiveShade(ReflectiveClass *self,
                                HitRecordStruct *hitRecord,
                                LightClass *currentLight);
static Color	reflectiveShadeRGB(ReflectiveClass *self,
                                   HitRecordStruct *hitRecord);
static Color	reflectiveShadePath(ReflectiveClass *self,
                                    HitRecordStruct *hitRecord);

static ReflectiveClass       _descr = { /* ReflectiveClass */
  { /* PhongClass */
    { /* MaterialClass */
      { /* Class */
        sizeof(ReflectiveClass),
        __RTTI_REFLECTIVE, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
        &reflectiveConstructor,
        &reflectiveDestructor,
      },
      /* public */
      &reflectiveShade, /* shade */
      &reflectiveShadeRGB,
      0, /* shadeAreaLight */
      &reflectiveShadePath,
      /* public */
      {0., 0., 0.}, /* color */
      /* protected */
      {
        0, /* normalBuffer */
        0, /* pointBuffer */
        0 /* rayBuffer */
      }
    },
    0, /* setters */
    0, /* setters */
    0, /* setters */
    { /* private */
      0, /* mDiffuseBrdf */
      0 /* mGlossySpecularBrdf */
    }
  },
  &reflectiveSetKr,
  &reflectiveSetCr,
  { /* private */
    0, /* mPerfectSpecular */
  }
};

Class *Reflective = (Class*) &_descr;

static void	reflectiveConstructor(ReflectiveClass *self, va_list *list)
{
  Phong->__constructor__(self, list);
  self->private.mPerfectSpecular = new(PerfectSpecular, eRegular);
  MATERIAL(self)->shadeRGB = &reflectiveShadeRGB;
  MATERIAL(self)->shadePath = &reflectiveShadePath;
}

static void	reflectiveDestructor(ReflectiveClass *self)
{
  delete(self->private.mPerfectSpecular);
}

void	*reflectiveUnserialize(SceneSerializerClass *serializer,
                           xmlNodePtr node)
{
  ReflectiveClass	*self;
  MaterialIdAssoc	*assoc;
  char			*strKr, *strCr;
  float			kr, cr;

  self = new(Reflective);
  if ((assoc = phongUnserializeSet(serializer, node, PHONG(self))) == 0)
    return (0);
  serializer->setStrsFromProp(serializer, node, "kr", &strKr, "cr",
      &strCr, NULL);
  if (setFloatFromStr(strKr, &kr) != 0 || setFloatFromStr(strCr, &cr) != 0)
  {
    fprintf(stderr, "Error: bad attributes\n");
    return (0);
  }
  self->setKr(self, kr);
  self->setCr(self, cr);
  return (assoc);
}

static void	reflectiveSetCr(ReflectiveClass *self, float cr)
{
  PERFECTSPEC(self->private.mPerfectSpecular)\
    ->setCr(PERFECTSPEC(self->private.mPerfectSpecular), cr);
}

static void	reflectiveSetKr(ReflectiveClass *self, float kr)
{
  PERFECTSPEC(self->private.mPerfectSpecular)\
    ->setKr(PERFECTSPEC(self->private.mPerfectSpecular), kr);
}

static float		reflectiveShade(ReflectiveClass *self,
                                   HitRecordStruct *hitRecord,
                                   LightClass *currentLight)
{
  return (MATERIAL(Phong)->shade(self, hitRecord, currentLight));
}

static Color		reflectiveShadeRGB(ReflectiveClass *self,
                                           HitRecordStruct *hitRecord)
{
  Color			color = {0, 0, 0};
  Vector3D		wo;
  Vector3D		wi;
  Color			fr;
  Color			tmp;
  RayStruct		reflectedRay;
  WhittedClass		*tracer;

  color = MATERIAL(Phong)->shadeRGB(self, hitRecord);
  wo = hitRecord->ray.direction;
  vectInv(&wo);
  fr = BRDFT(self->private.mPerfectSpecular)
    ->sampleF(self->private.mPerfectSpecular, hitRecord, &wo, &wi);
  reflectedRay = (RayStruct) { hitRecord->hitPoint, wi };
  tracer = WHITTED(TracerManagerGetTracer(eWhitted));
  tmp = tracer->trace(tracer, &reflectedRay, hitRecord->reflectDepth + 1);
  colorMult(&tmp, vectDotProduct(&hitRecord->normal, &wi));
  colorMultColor(&fr, &tmp);
  colorAdd(&color, &fr);
  fr = MATERIAL(self)->shadeAreaLight(self, hitRecord);
  colorAdd(&color, &fr);
  return (color);
}

static Color		reflectiveShadePath(ReflectiveClass *self,
                                            HitRecordStruct *hitRecord)
{
  Vector3D		wi;
  Vector3D		wo;
  float			pdf;
  Color			fr;
  RayStruct		reflray;

  wo = hitRecord->ray.direction;
  vectInv(&wo);
  fr = BRDFT(PRIV(mPerfectSpecular))->sampleFPath(
         BRDFT(PRIV(mPerfectSpecular)), hitRecord, &wo, &wi, &pdf);
  reflray = (RayStruct){hitRecord->hitPoint, wi};
  colorMultColor(&fr,
   COLORPTR(PATHTRACER(TracerManagerGetTracer(ePathTracer))\
    ->trace(PATHTRACER(TracerManagerGetTracer(ePathTracer)), &reflray,
      hitRecord->reflectDepth + 1)));
  colorMult(&fr, vectDotProduct(&hitRecord->normal, &wi) / pdf);
  colorMultColor(&fr, &(MATERIAL(self)->color));
  return (fr);
}

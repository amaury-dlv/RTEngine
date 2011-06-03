#include "GlossyReflective.h"
#include "Lambertian.h"
#include "GlossySpecular.h"
#include "Utils.h"
#include "TracerManager.h"
#include "Whitted.h"
#include "PathTracer.h"

static void	glossyReflectiveConstructor();
static void	glossyReflectiveDestructor(GlossyReflectiveClass *self);
static void	glossyReflectiveSetCr(GlossyReflectiveClass *self, float cr);
static float	glossyReflectiveShade(GlossyReflectiveClass *self,
                                HitRecordStruct *hitRecord,
                                LightClass *currentLight);
static Color	glossyReflectiveShadeRGB(GlossyReflectiveClass *self,
                                   HitRecordStruct *hitRecord);
static Color	glossyReflectiveShadePath(GlossyReflectiveClass *self,
                                    HitRecordStruct *hitRecord);

static GlossyReflectiveClass       _descr = { /* GlossyReflectiveClass */
  { /* PhongClass */
    { /* MaterialClass */
      { /* Class */
        sizeof(GlossyReflectiveClass),
        __RTTI_GLOSSYREFLECTIVE, /* RTTI */
        &glossyReflectiveConstructor,
        &glossyReflectiveDestructor,
      },
      /* public */
      &glossyReflectiveShade, /* shade */
      &glossyReflectiveShadeRGB,
      0, /* shadeAreaLight */
      &glossyReflectiveShadePath,
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
  &glossyReflectiveSetCr
};

Class *GlossyReflective = (Class*) &_descr;

static void	glossyReflectiveConstructor(GlossyReflectiveClass *self,
   va_list *list)
{
  Phong->__constructor__(self, list);
  MATERIAL(self)->shadeRGB = &glossyReflectiveShadeRGB;
  MATERIAL(self)->shadePath = &glossyReflectiveShadePath;
}

static void	glossyReflectiveDestructor(GlossyReflectiveClass *self)
{
}

void	*glossyReflectiveUnserialize(SceneSerializerClass *serializer,
                           xmlNodePtr node)
{
  GlossyReflectiveClass	*self;
  MaterialIdAssoc	*assoc;
  char			*strCr;
  float			cr;

  self = new(GlossyReflective);
  if ((assoc = phongUnserializeSet(serializer, node, PHONG(self))) == 0)
    return (0);
  serializer->setStrsFromProp(serializer, node, "cr",
      &strCr, NULL);
  if (setFloatFromStr(strCr, &cr) != 0)
  {
    fprintf(stderr, "Error: bad attributes\n");
    return (0);
  }
  self->setCr(self, cr);
  return (assoc);
}

static void	glossyReflectiveSetCr(GlossyReflectiveClass *self, float cr)
{
  GLOSSYSPEC(PHONG(self)->private.mGlossySpecularBrdf)\
    ->setCr(GLOSSYSPEC(PHONG(self)->private.mGlossySpecularBrdf), cr);
}

static float		glossyReflectiveShade(GlossyReflectiveClass *self,
                                   HitRecordStruct *hitRecord,
                                   LightClass *currentLight)
{
  return (MATERIAL(Phong)->shade(self, hitRecord, currentLight));
}

static Color		glossyReflectiveShadeRGB(GlossyReflectiveClass *self,
                                           HitRecordStruct *hitRecord)
{
  Color			color;
  Vector3D		wo;
  Vector3D		wi;
  Color			fr;
  Color			tmp;
  RayStruct		reflectedRay;
  WhittedClass		*tracer;

  color = MATERIAL(Phong)->shadeRGB(self, hitRecord);
  wo = hitRecord->ray.direction;
  vectInv(&wo);
  fr = BRDFT(PHONG(self)->private.mGlossySpecularBrdf)
    ->sampleF(PHONG(self)->private.mGlossySpecularBrdf, hitRecord, &wo, &wi);
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

static Color		glossyReflectiveShadePath(GlossyReflectiveClass *self,
                                            HitRecordStruct *hitRecord)
{
  Vector3D		wi;
  Vector3D		wo;
  float			pdf;
  Color			fr;
  RayStruct		reflray;

  wo = hitRecord->ray.direction;
  vectInv(&wo);
  fr = BRDFT(PHONG(self)->private.mGlossySpecularBrdf)\
       ->sampleFPath(BRDFT(PHONG(self)->private.mGlossySpecularBrdf),
                     hitRecord, &wo, &wi, &pdf);
  reflray = (RayStruct){hitRecord->hitPoint, wi};
  colorMultColor(&fr, COLORPTR(PATHTRACER(TracerManagerGetTracer(ePathTracer))\
    ->trace(PATHTRACER(TracerManagerGetTracer(ePathTracer)),
	    &reflray, hitRecord->reflectDepth + 1)));
  colorMult(&fr, vectDotProduct(&hitRecord->normal, &wi) / pdf);
  colorMultColor(&fr, &MATERIAL(self)->color);
  return (fr);
}

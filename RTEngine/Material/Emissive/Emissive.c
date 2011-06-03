#include "RTTI.h"
#include "Emissive.h"
#include "Lambertian.h"
#include "HitRecord.h"
#include "SceneSerializer.h"
#include "Utils.h"
#include "RenderingScene.h"

static void	emissiveConstructor(EmissiveClass *self, va_list *list);
static void	emissiveDestructor(EmissiveClass *self);
static void	emissiveScaleRadiance(EmissiveClass *self, float ls);
static float	emissiveShade(EmissiveClass *self, HitRecordStruct *hitRecord,
                           LightClass *currentLight);
static Color		emissiveShadeRGB(EmissiveClass *self,
                                      HitRecordStruct *hitRecord);
static Color		emissiveGetLe(EmissiveClass *self);
static Color	emissiveShadePath(EmissiveClass *self,
                                       HitRecordStruct *hitRecord);

static EmissiveClass       _descr = { /* EmissiveClass */
    { /* MaterialClass */
      { /* Class */
        sizeof(EmissiveClass),
        __RTTI_EMISSIVE, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
        &emissiveConstructor,
        &emissiveDestructor,
      },
      /* public */
      &emissiveShade, /* shade */
      &emissiveShadeRGB, /* shadeRGB() */
      &emissiveShadeRGB, /* shadeAreaLight() */
      &emissiveShadePath, /* shadePath */
      /* public */
      {0., 0., 0.}, /* color */
      /* protected */
      {
        0, /* normalBuffer */
        0, /* pointBuffer */
        0 /* rayBuffer */
      }
    },
    emissiveScaleRadiance, /* scaleRadiance() */
    emissiveGetLe, /* getLe() */
    { /* private */
      0 /* mLs */
    }
  };

Class *Emissive = (Class*) &_descr;

static void	emissiveConstructor(EmissiveClass *self, va_list *list)
{
  Material->__constructor__(self, list);
}

void	*emissiveUnserialize(SceneSerializerClass *serializer,
                           xmlNodePtr node)
{
  EmissiveClass		*self;
  MaterialIdAssoc	*assoc;
  char			*strId;
  char			*strLs;
  float			ls;

  self = new(Emissive);
  if (materialUnserializeSet(serializer, node, self) != 0)
    return (0);
  serializer->setStrsFromProp(serializer, node, "id", &strId,
		  "ls", &strLs, NULL);
  assoc = xmalloc(sizeof(*assoc));
  *assoc = (MaterialIdAssoc) {false, strId, MATERIAL(self)};
  if (setFloatFromStr(strLs, &ls) != 0)
  {
    fprintf(stderr, "Error: bad attributes\n");
    return (0);
  }
  self->scaleRadiance(self, ls);
  return (assoc);
}

static void	emissiveDestructor(EmissiveClass *self)
{
}

static float		emissiveShade(EmissiveClass *self, HitRecordStruct *hitRecord,
                                   LightClass *currentLight)
{
  Vector3D	normal;

  normal = hitRecord->normal;
  vectInv(&normal);
  if (vectDotProduct(&normal, &hitRecord->ray.direction) > 0.)
    return (PRIV(mLs));
  return (0.);
}

static Color		emissiveShadeRGB(EmissiveClass *self,
                                      HitRecordStruct *hitRecord)
{
  Color			color;

  color = MATERIAL(self)->color;
  colorMult(&color, emissiveShade(self, hitRecord, NULL));
  return (color);
}


static Color		emissiveGetLe(EmissiveClass *self)
{
  Color			color;

  color = MATERIAL(self)->color;
  colorMult(&color, PRIV(mLs));
  return (color);
}

static void	emissiveScaleRadiance(EmissiveClass *self, float ls)
{
  PRIV(mLs) = ls;
}

static Color	emissiveShadePath(EmissiveClass *self,
                                       HitRecordStruct *hitRecord)
{
  return (emissiveShadeRGB(self, hitRecord));
}

#include <string.h>

#include "Checker.h"
#include "Lambertian.h"
#include "GlossySpecular.h"
#include "Utils.h"
#include "TracerManager.h"
#include "Whitted.h"
#include "Phong.h"
#include "Color.h"
#include "Transparent.h"

static void	checkerConstructor(CheckerClass *self, va_list *list);
static void	checkerDestructor(CheckerClass *self);
static void	checkerChecker3D(CheckerClass *self,
				 HitRecordStruct *hitRecord,
				 Color *color);
static float	checkerShade(CheckerClass *self,
                             HitRecordStruct *hitRecord,
                             LightClass *currentLight);
static Color	checkerShadeRGB(CheckerClass *self,
                                HitRecordStruct *hitRecord);

static CheckerClass       _descr = { /* CheckerClass */
    { /* TransparentClass */
      { /* PhongClass */
	{ /* MaterialClass */
	  { /* Class */
	    sizeof(CheckerClass),
	    __RTTI_CHECKER, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
	    &checkerConstructor,
	    &checkerDestructor,
	  },
	  /* public */
	  /* public */
	  &checkerShade, /*  */
	  &checkerShadeRGB,
	  0, /* shadeAreaLight */
	  0, /* shadePath */
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
      0,  /* SetKr */
      0,  /* SetCr */
      0,  /* SetKt */
      0,  /* SetIor */
      { /* private */
	0,
	0,
      }
    },
    &checkerChecker3D,
    { /* private */
      0, /* mInsideColor */
      0, /* mSize */
    }
  };

Class *Checker = (Class*) &_descr;

static void	checkerConstructor(CheckerClass *self, va_list *list)
{
  Transparent->__constructor__(self, list);
  MATERIAL(self)->shade = &checkerShade;
  MATERIAL(self)->shadeRGB = &checkerShadeRGB;
}

static void	checkerDestructor(CheckerClass *self)
{
}

void	*checkerUnserialize(SceneSerializerClass *serializer,
			    xmlNodePtr node)
{
  CheckerClass		*self;
  MaterialIdAssoc	*assoc;
  char			*strSize, *strColor2;
  float			size;

  self = new(Checker);
  if ((assoc = transparentUnserializeSet(serializer,
               node, TRANSPARENT(self))) == 0)
    return (0);
  serializer->setStrsFromProp(serializer, node, "size", &strSize, NULL);
  if (setFloatFromStr(strSize, &size) != 0)
  {
    fprintf(stderr, "Error: bad attributes\n");
    return (0);
  }
  strColor2 = (char*) xmlGetProp(node, XMLSTR("color2"));
  if (strColor2 != 0)
  {
    self->private.mInsideColor = malloc(sizeof(Color));
    memset(self->private.mInsideColor, 0, sizeof(Color));
    if (setColorFromStr(strColor2, self->private.mInsideColor) != 0)
      return (0);
  }
  self->private.mSize = size;
  return (assoc);
}

static float		checkerShade(CheckerClass *self,
                                   HitRecordStruct *hitRecord,
                                   LightClass *currentLight)
{
  return (MATERIAL(Transparent)->shade(self, hitRecord, currentLight));
}

static Color		checkerShadeRGB(CheckerClass *self,
                                           HitRecordStruct *hitRecord)
{
  Color			color = {0, 0, 0};

  color = MATERIAL(Transparent)->shadeRGB(self, hitRecord);
  self->checker3D(self, hitRecord, &color);
  return (color);
}

static void	checkerChecker3D(CheckerClass *self,
			 HitRecordStruct *hitRecord, Color *color)
{
  float x;
  float y;
  float z;
  float size;
  float eps;

  eps = -0.000187453738;
  x = hitRecord->hitPoint.x + eps;
  y = hitRecord->hitPoint.y + eps;
  z = hitRecord->hitPoint.z + eps;
  size = self->private.mSize;
  if (((int)floor(x / size)
       + (int)floor(y / size)
       + (int)floor(z / size)) % 2 == 0)
  {
    if (self->private.mInsideColor)
      colorMultColor(color, self->private.mInsideColor);
  }
}

#include <string.h>

#include "Bump.h"
#include "Lambertian.h"
#include "GlossySpecular.h"
#include "Utils.h"
#include "TracerManager.h"
#include "Whitted.h"
#include "PerlinNoise.h"

static void	bumpConstructor(BumpClass *self, va_list *list);
static void	bumpDestructor(BumpClass *self);
static float	bumpShade(BumpClass *self,
			  HitRecordStruct *hitRecord,
			  LightClass *currentLight);
static Color	bumpShadeRGB(BumpClass *self,
			     HitRecordStruct *hitRecord);
static void	bumpSetBumpType(BumpClass *self, char *strBump);
static void	bumpCaseBasic(BumpClass *self, HitRecordStruct *hitRecord,
                              Vector3D *normalPtr);
static void	bumpCaseBasic2(BumpClass *self, HitRecordStruct *hitRecord,
                               Vector3D *normalPtr);
static void	bumpCaseUniform(BumpClass *self, HitRecordStruct *hitRecord,
                                Vector3D *normalPtr);
static void	bumpCaseWaves(BumpClass *self, HitRecordStruct *hitRecord,
                              Vector3D *normalPtr);
static void	bumpCaseSoftWaves(BumpClass *self, HitRecordStruct *hitRecord,
                                  Vector3D *normalPtr);
static void	bumpCaseHardWaves(BumpClass *self, HitRecordStruct *hitRecord,
                                  Vector3D *normalPtr);
static void	bumpCaseSphere1(BumpClass *self, HitRecordStruct *hitRecord,
                                Vector3D *normalPtr);
static void	bumpCaseSphere2(BumpClass *self, HitRecordStruct *hitRecord,
                                Vector3D *normalPtr);
static void	bumpCaseSphere3(BumpClass *self, HitRecordStruct *hitRecord,
                                Vector3D *normalPtr);
static void	bumpCaseExperimental(BumpClass *self, HitRecordStruct *hitRecord,
                                     Vector3D *normalPtr);
static void	bumpCaseLol1(BumpClass *self, HitRecordStruct *hitRecord,
                             Vector3D *normalPtr);

static BumpClass       _descr = { /* BumpClass */
  { /* TransparentClass */
    { /* PhongClass */
      { /* MaterialClass */
	{ /* Class */
	  sizeof(BumpClass),
	  __RTTI_BUMP, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
	  &bumpConstructor,
	  &bumpDestructor,
	},
	/* public */
	&bumpShade, /* phongShade */
	&bumpShadeRGB,
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
  &bumpSetBumpType,
  0, /* bumpBumpNormal() = 0 */
  { /* private */
  }
};

Class *Bump = (Class*) &_descr;

static void	bumpConstructor(BumpClass *self, va_list *list)
{
  Transparent->__constructor__(self, list);
  MATERIAL(self)->shade = &bumpShade;
  MATERIAL(self)->shadeRGB = &bumpShadeRGB;
}

static void	bumpDestructor(BumpClass *self)
{
}

void	*bumpUnserialize(SceneSerializerClass *serializer,
                           xmlNodePtr node)
{
  BumpClass		*self;
  MaterialIdAssoc	*assoc;
  char			*strType;

  self = new(Bump);
  if ((assoc = transparentUnserializeSet(serializer, node,
					 TRANSPARENT(self))) == 0)
    return (0);
  serializer->setStrsFromProp(serializer, node, "bumpt", &strType, NULL);
  self->setBumpType(self, strType);
  return (assoc);
}

static void	bumpSetBumpType(BumpClass *self, char *strBump)
{
  if (!strcmp("basic", strBump))
    self->bumpNormal = &bumpCaseBasic;
  if (!strcmp("basic2", strBump))
    self->bumpNormal = &bumpCaseBasic2;
  if (!strcmp("uniform", strBump))
    self->bumpNormal = &bumpCaseUniform;
  if (!strcmp("waves", strBump))
    self->bumpNormal = &bumpCaseWaves;
  if (!strcmp("soft_waves", strBump))
    self->bumpNormal = &bumpCaseSoftWaves;
  if (!strcmp("hard_waves", strBump))
    self->bumpNormal = &bumpCaseHardWaves;
  if (!strcmp("sphee1", strBump))
    self->bumpNormal = &bumpCaseSphere1;
  if (!strcmp("sphere2", strBump))
    self->bumpNormal = &bumpCaseSphere2;
  if (!strcmp("sphere3", strBump))
    self->bumpNormal = &bumpCaseSphere3;
  if (!strcmp("experimental", strBump))
    self->bumpNormal = &bumpCaseExperimental;
  if (!strcmp("lol1", strBump))
    self->bumpNormal = &bumpCaseLol1;
}

static float		bumpShade(BumpClass *self,
                                   HitRecordStruct *hitRecord,
                                   LightClass *currentLight)
{
  if (self->bumpNormal)
    self->bumpNormal(self, hitRecord, &hitRecord->normal);
  return (MATERIAL(Transparent)->shade(self, hitRecord, currentLight));
}

static Color		bumpShadeRGB(BumpClass *self,
				     HitRecordStruct *hitRecord)
{
  if (self->bumpNormal)
    self->bumpNormal(self, hitRecord, &hitRecord->normal);
  return (MATERIAL(Transparent)->shadeRGB(self, hitRecord));
}

static void	bumpCaseBasic(BumpClass *self,
			      HitRecordStruct *hitRecord,
			      Vector3D *normalPtr)
{
  normalPtr->y = 0.20 * hitRecord->normal.y
    + cos(hitRecord->hitPoint.y / 10.0)
    * vectLength(&hitRecord->normal) / 10.0;
  vectNormalize(normalPtr);
}

static void	bumpCaseUniform(BumpClass *self,
				HitRecordStruct *hitRecord,
				Vector3D *normalPtr)
{
  Vector3D	noiseValue;
  const float	frequency = 0.00023;
  const float	bump = 0.017;

  noiseValue.y = cos(hitRecord->hitPoint.x
		     + pnoise(hitRecord->hitPoint.x * frequency,
			      hitRecord->hitPoint.y * frequency,
			      hitRecord->hitPoint.z * frequency));
  normalPtr->y = (1.0f - bump) * hitRecord->normal.y + bump * noiseValue.y;
  vectNormalize(normalPtr);
}

static void	bumpCaseWaves(BumpClass *self,
			      HitRecordStruct *hitRecord,
			      Vector3D *normalPtr)
{
  Vector3D	noiseValue;
  const float	frequency = 0.23;
  const float	bump = 0.07;

  noiseValue.x = cos(hitRecord->hitPoint.x
		     + pnoise(hitRecord->hitPoint.x * frequency,
			      hitRecord->hitPoint.y * frequency,
			      hitRecord->hitPoint.z * frequency));
  normalPtr->x = (1.0f - bump) * hitRecord->normal.x + bump * noiseValue.x;
  vectNormalize(normalPtr);
}

static void	bumpCaseSoftWaves(BumpClass *self,
				  HitRecordStruct *hitRecord,
				  Vector3D *normalPtr)
{
  Vector3D	noiseValue;
  const float	frequency = 0.0013;
  const float	bump = 0.007;

  noiseValue.x = pnoise(hitRecord->hitPoint.x * frequency,
			hitRecord->hitPoint.y * frequency,
			hitRecord->hitPoint.z * frequency);
  noiseValue.y = pnoise(hitRecord->hitPoint.x * frequency,
			hitRecord->hitPoint.y * frequency,
			hitRecord->hitPoint.z * frequency);
  noiseValue.z = pnoise(hitRecord->hitPoint.x * frequency,
			hitRecord->hitPoint.y * frequency,
			hitRecord->hitPoint.z * frequency);
  noiseValue.x *= cos(hitRecord->hitPoint.x + noiseValue.x);
  noiseValue.y *= cos(hitRecord->hitPoint.y + noiseValue.y);
  noiseValue.z *= cos(hitRecord->hitPoint.z + noiseValue.z);
  normalPtr->x = (1.0f - bump) * hitRecord->normal.x + bump * noiseValue.x;
  normalPtr->y = (1.0f - bump) * hitRecord->normal.y + bump * noiseValue.y;
  normalPtr->z = (1.0f - bump) * hitRecord->normal.z + bump * noiseValue.z;
  vectNormalize(normalPtr);
}

static void	bumpCaseHardWaves(BumpClass *self,
				  HitRecordStruct *hitRecord,
				  Vector3D *normalPtr)
{
  Vector3D	noiseValue;

  noiseValue.x = 0.025 * pnoise((hitRecord->hitPoint.x - EPS01) * FREQ01,
  hitRecord->hitPoint.y * FREQ01, hitRecord->hitPoint.z * FREQ01)
  - pnoise((hitRecord->hitPoint.x + EPS01) * FREQ01,
  hitRecord->hitPoint.y * FREQ01, hitRecord->hitPoint.z * FREQ01);
  noiseValue.y = pnoise(hitRecord->hitPoint.x * FREQ01,
  (hitRecord->hitPoint.y - EPS01) * FREQ01, hitRecord->hitPoint.z * FREQ01)
  - pnoise(hitRecord->hitPoint.x * FREQ01,  (hitRecord->hitPoint.y + EPS01)
  * FREQ01, hitRecord->hitPoint.z * FREQ01);
  noiseValue.z = pnoise(hitRecord->hitPoint.x * FREQ01,
  hitRecord->hitPoint.y * FREQ01, (hitRecord->hitPoint.z - EPS01) * FREQ01)
  - pnoise(hitRecord->hitPoint.x * FREQ01, hitRecord->hitPoint.y
  * FREQ01, (hitRecord->hitPoint.z + EPS01) * FREQ01);
  normalPtr->x = hitRecord->normal.x + noiseValue.x / 20.0;
  normalPtr->y = hitRecord->normal.y + noiseValue.y / 20.0;
  normalPtr->z = hitRecord->normal.z + noiseValue.z / 20.0;
  vectNormalize(normalPtr);
}

static void	bumpCaseBasic2(BumpClass *self,
			       HitRecordStruct *hitRecord,
			       Vector3D *normalPtr)
{
  Vector3D	noiseValue;
  const float	frequency = 0.23;
  const float	bump = 0.07;

  noiseValue.x = cos(hitRecord->hitPoint.x
		     + pnoise(hitRecord->hitPoint.x * frequency,
			      hitRecord->hitPoint.y * frequency,
			      hitRecord->hitPoint.z * frequency));
  noiseValue.y = cos(hitRecord->hitPoint.y
		     + pnoise(hitRecord->hitPoint.x * frequency,
			      hitRecord->hitPoint.y * frequency,
			      hitRecord->hitPoint.z * frequency));
  noiseValue.z = cos(hitRecord->hitPoint.z
		     + pnoise(hitRecord->hitPoint.x * frequency,
			      hitRecord->hitPoint.y * frequency,
			      hitRecord->hitPoint.z * frequency));
  normalPtr->x = (1.0f - bump) * hitRecord->normal.x + bump * noiseValue.x;
  normalPtr->y = (1.0f - bump) * hitRecord->normal.y + bump * noiseValue.x;
  normalPtr->z = (1.0f - bump) * hitRecord->normal.z + bump * noiseValue.z;
  vectNormalize(normalPtr);
}

static void	bumpCaseSphere1(BumpClass *self,
				HitRecordStruct *hitRecord,
				Vector3D *normalPtr)
{
  Vector3D	noiseValue;
  const float	frequency = 0.003;
  const float	bump = 1.7;

  noiseValue.z = cos(hitRecord->hitPoint.z * 2.0
  		     + pnoise(hitRecord->hitPoint.x * frequency,
  			      hitRecord->hitPoint.y * frequency,
  			      hitRecord->hitPoint.z * frequency));
  normalPtr->z = (1.0f - bump) * hitRecord->normal.z + bump * noiseValue.z;
  vectNormalize(normalPtr);
}

static void	bumpCaseSphere2(BumpClass *self,
				HitRecordStruct *hitRecord,
				Vector3D *normalPtr)
{
  Vector3D	noiseValue;
  const float	frequency = 0.3;
  const float	bump = 1.7;

  noiseValue.x = cos(hitRecord->hitPoint.x * 20.0
  		     + pnoise(hitRecord->hitPoint.x * frequency,
  			      hitRecord->hitPoint.y * frequency,
  			      hitRecord->hitPoint.z * frequency));
  noiseValue.y = cos(hitRecord->hitPoint.y * 20.0
  		     + pnoise(hitRecord->hitPoint.x * frequency,
  			      hitRecord->hitPoint.y * frequency,
  			      hitRecord->hitPoint.z * frequency));
  noiseValue.z = cos(hitRecord->hitPoint.z * 20.0
  		     + pnoise(hitRecord->hitPoint.x * frequency,
  			      hitRecord->hitPoint.y * frequency,
  			      hitRecord->hitPoint.z * frequency));
  normalPtr->x = hitRecord->normal.x + bump + noiseValue.x;
  normalPtr->y = hitRecord->normal.y + bump + noiseValue.y;
  normalPtr->z = hitRecord->normal.z + bump + noiseValue.z;
  vectNormalize(normalPtr);
}

static void	bumpCaseSphere3(BumpClass *self,
				HitRecordStruct *hitRecord,
				Vector3D *normalPtr)
{
  Vector3D	noiseValue;
  const float	frequency = 0.3;
  const float	bump = 0.7;

  noiseValue.x = cos(hitRecord->hitPoint.x * 5.0
  		     + pnoise(hitRecord->hitPoint.x * frequency,
  			      hitRecord->hitPoint.y * frequency,
  			      hitRecord->hitPoint.z * frequency));
  noiseValue.y = cos(hitRecord->hitPoint.y * 10.0
  		     + pnoise(hitRecord->hitPoint.x * frequency,
  			      hitRecord->hitPoint.y * frequency,
  			      hitRecord->hitPoint.z * frequency));
  noiseValue.z = cos(hitRecord->hitPoint.z * 0.02
  		     + pnoise(hitRecord->hitPoint.x * frequency,
  			      hitRecord->hitPoint.y * frequency,
  			      hitRecord->hitPoint.z * frequency));
  normalPtr->x = hitRecord->normal.x + bump + noiseValue.x;
  normalPtr->y = hitRecord->normal.y + bump + noiseValue.y;
  normalPtr->z = hitRecord->normal.z + bump + noiseValue.z;
  vectNormalize(normalPtr);
}

/*  */
static void	bumpCaseExperimental(BumpClass *self,
				     HitRecordStruct *hitRecord,
				     Vector3D *normalPtr)
{
  Vector3D	noiseValue;

  noiseValue.x = 10.2 * pnoise((hitRecord->hitPoint.x - EPS01) * FREQ02,
  hitRecord->hitPoint.y * FREQ02, hitRecord->hitPoint.z * FREQ02)
  - pnoise((hitRecord->hitPoint.x + EPS01) * FREQ02,
  hitRecord->hitPoint.y * FREQ02, hitRecord->hitPoint.z * FREQ02);
  noiseValue.y = 0.2 * pnoise(hitRecord->hitPoint.x * FREQ02,
  (hitRecord->hitPoint.y - EPS01) * FREQ02, hitRecord->hitPoint.z * FREQ02)
  - pnoise(hitRecord->hitPoint.x * FREQ02,
  (hitRecord->hitPoint.y + EPS01) * FREQ02,
  hitRecord->hitPoint.z * FREQ02);
  noiseValue.z = 20.20 * pnoise(hitRecord->hitPoint.x * FREQ02,
  hitRecord->hitPoint.y * FREQ02, (hitRecord->hitPoint.z - EPS01) * FREQ02)
  - pnoise(hitRecord->hitPoint.x * FREQ02, hitRecord->hitPoint.y
  * FREQ02, (hitRecord->hitPoint.z + EPS01) * FREQ02);
  normalPtr->x = hitRecord->normal.x + noiseValue.x / 20.0;
  normalPtr->y = hitRecord->normal.y + noiseValue.y / 20.0;
  normalPtr->z = hitRecord->normal.z + noiseValue.z / 20.0;
  vectNormalize(normalPtr);
}

/*  */
static void	bumpCaseLol1(BumpClass *self,
			     HitRecordStruct *hitRecord,
			     Vector3D *normalPtr)
{
  Vector3D	noiseValue;

  noiseValue.x = pnoise(hitRecord->hitPoint.x - EPS01 * FREQ03,
     hitRecord->hitPoint.y * FREQ03, hitRecord->hitPoint.z)
   - pnoise(hitRecord->hitPoint.x + EPS01 * FREQ03, hitRecord->\
     hitPoint.y * FREQ03, hitRecord->hitPoint.z);
  noiseValue.y = pnoise(hitRecord->hitPoint.x * FREQ03,
    hitRecord->hitPoint.y - EPS01 * FREQ03, hitRecord->hitPoint.z)
  - pnoise(hitRecord->hitPoint.x * FREQ03, hitRecord->\
    hitPoint.y + EPS01 * FREQ03, hitRecord->hitPoint.z);
  noiseValue.z = pnoise(hitRecord->hitPoint.x * FREQ03,
    hitRecord->hitPoint.y * FREQ03, hitRecord->hitPoint.z - EPS01)
  - pnoise(hitRecord->hitPoint.x * FREQ03, hitRecord->\
    hitPoint.y * FREQ03, hitRecord->hitPoint.z + EPS01);
  noiseValue.x += cos(hitRecord->hitPoint.x + noiseValue.x)
                / sin(noiseValue.x);
  noiseValue.y += cos(hitRecord->hitPoint.y + noiseValue.y)
               / sin(noiseValue.y);
  noiseValue.z += cos(hitRecord->hitPoint.z + noiseValue.z)
               / sin(noiseValue.z);
  normalPtr->x = (noiseValue.x - 0.15) / (0.75 - 0.15);
  normalPtr->y = (noiseValue.y - 0.15) / (0.75 - 0.15);
  normalPtr->z = (noiseValue.z - 0.15) / (0.75 - 0.15);
  vectNormalize(normalPtr);
}

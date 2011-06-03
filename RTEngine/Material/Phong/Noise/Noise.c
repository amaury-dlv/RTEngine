#include <string.h>

#include "Noise.h"
#include "Lambertian.h"
#include "GlossySpecular.h"
#include "Utils.h"
#include "TracerManager.h"
#include "Whitted.h"
#include "Color.h"
#include "PerlinNoise.h"

static void	noiseConstructor(NoiseClass *self, va_list *list);
static void	noiseDestructor(NoiseClass *self);
static void	noiseSetCr(NoiseClass *self, float cr);
static void	noiseSetKr(NoiseClass *self, float kr);
static float	noiseShade(NoiseClass *self,
                             HitRecordStruct *hitRecord,
                             LightClass *currentLight);
static Color	noiseShadeRGB(NoiseClass *self,
                                HitRecordStruct *hitRecord);

static void	noiseSetNoiseType(NoiseClass *self, char *strNoiseType);
static float	noiseApplyTurbulence(NoiseClass *self, Point3D *hitPoint);
static float	noiseApplyfBm(NoiseClass *self, Point3D *hitPoint);
static float	noiseGetColor(NoiseClass *self, HitRecordStruct *hitRecord);
static void	noiseComputefBmBounds(NoiseClass *self);
static float	noiseApplyCosX(NoiseClass *self, Point3D *hitPoint);
static float	noiseApplyCosY(NoiseClass *self, Point3D *hitPoint);

static NoiseClass       _descr = { /* NoiseClass */
  { /* PhongClass */
    { /* MaterialClass */
      { /* Class */
        sizeof(NoiseClass),
        __RTTI_NOISE, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
        &noiseConstructor,
        &noiseDestructor,
      },
      /* public */
      &noiseShade, /* phongShade */
      &noiseShadeRGB,
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
  &noiseSetKr,
  &noiseSetCr,
  &noiseSetNoiseType,
  &noiseGetColor,
  &noiseComputefBmBounds,
  0, /* applyNoise (virtual) */
  { /* private */
    0, /* mPerfectSpecular */
    0, /* mNumOctaves */
    0, /* mGain */
    0, /* mLacunarity */
    0, /* mMax */
    0, /* mMin */
    0, /* mfBmMax */
    0, /* mfBmMin */
    0, /* mfsMax */
    0, /* mfsMin */
    0, /* exp */
  }
};

Class *Noise = (Class*) &_descr;

static void	noiseConstructor(NoiseClass *self, va_list *list)
{
  Phong->__constructor__(self, list);
  self->private.mPerfectSpecular = new(PerfectSpecular, eRegular);
  MATERIAL(self)->shadeRGB = &noiseShadeRGB;
}

static void	noiseDestructor(NoiseClass *self)
{
  delete(self->private.mPerfectSpecular);
}

void	UnserializeSetNoise(NoiseClass *self, NoiseUnserializeStruct *n)
{
  self->setKr(self, n->kr);
  self->setCr(self, n->cr);
  self->setNoiseType(self, n->strNoise);
  self->private.mNumOctaves = n->octaves;
  self->private.mGain = n->gain;
  self->private.mMax = n->min;
  self->private.mMin = n->max;
  self->private.mLacunarity = n->lacunarity;
  self->private.exp = n->exp;
}

void	*noiseUnserialize(SceneSerializerClass *serializer,
			  xmlNodePtr node)
{
  NoiseClass			*self;
  MaterialIdAssoc		*assoc;
  NoiseUnserializeStruct	n;

  self = new(Noise);
  if ((assoc = phongUnserializeSet(serializer, node, PHONG(self))) == 0)
    return (0);
  serializer->setStrsFromProp(serializer, node, "kr", &n.strKr, "cr", &n.strCr,
      "noiset", &n.strNoise, "octaves", &n.strOctaves,
      "lacunarity", &n.strLacunarity, "gain", &n.strGain, "min", &n.strMin,
      "max", &n.strMax, "e", &n.strExp, NULL);
  if (setFloatFromStr(n.strKr, &n.kr) != 0
      || setFloatFromStr(n.strCr, &n.cr) != 0
      || setFloatFromStr(n.strOctaves, &n.octaves) != 0
      || setFloatFromStr(n.strGain, &n.gain) != 0
      || setFloatFromStr(n.strGain, &n.min) != 0
      || setFloatFromStr(n.strGain, &n.max) != 0
      || setFloatFromStr(n.strLacunarity, &n.lacunarity) != 0
      || setFloatFromStr(n.strExp, &n.exp) != 0)
  {
    fprintf(stderr, "Error: bad attributes\n");
    return (0);
  }
  UnserializeSetNoise(self, &n);
  return (assoc);
}

static void	noiseSetCr(NoiseClass *self, float cr)
{
  PERFECTSPEC(self->private.mPerfectSpecular)\
    ->setCr(PERFECTSPEC(self->private.mPerfectSpecular), cr);
}

static void	noiseSetKr(NoiseClass *self, float kr)
{
  PERFECTSPEC(self->private.mPerfectSpecular)\
    ->setKr(PERFECTSPEC(self->private.mPerfectSpecular), kr);
}

static void	noiseSetNoiseType(NoiseClass *self, char *strNoiseType)
{
  if (!strcmp("fbm", strNoiseType))
    self->applyNoise = &noiseApplyfBm;
  if (!strcmp("turbulence", strNoiseType))
    self->applyNoise = &noiseApplyTurbulence;
  if (!strcmp("cosX", strNoiseType))
    self->applyNoise = &noiseApplyCosX;
  if (!strcmp("cosY", strNoiseType))
    self->applyNoise = &noiseApplyCosY;
}

static float		noiseShade(NoiseClass *self,
                                   HitRecordStruct *hitRecord,
                                   LightClass *currentLight)
{
  return (MATERIAL(Phong)->shade(self, hitRecord, currentLight));
}

static Color		noiseShadeRGB(NoiseClass *self,
                                           HitRecordStruct *hitRecord)
{
  Color			color = {0, 0, 0};

  color = MATERIAL(Phong)->shadeRGB(self, hitRecord);
  if (self->applyNoise)
  {
    self->computefBmBounds(self);
    if (self->private.exp)
    {
      float noise = self->private.exp * self->applyNoise(self, &hitRecord->hitPoint);
      noise = noise - floor(noise);
      colorMult(&color, noise);
    }
    else
      colorMult(&color, self->applyNoise(self, &hitRecord->hitPoint));
  }
  return (color);
}

static float	noiseApplyTurbulence(NoiseClass *self, Point3D *hitPoint)
{
  float amplitude = 1.0;
  float frequency = 1.0;
  float turbulence = 0.0;
  float fs_max;
  int	j;

  j = 0;
  fs_max = self->private.mfsMax;
  while (j < self->private.mNumOctaves)
  {
    turbulence += amplitude * fabsf(pnoise(hitPoint->x * frequency,
					   hitPoint->y * frequency,
					   hitPoint->z * frequency));
    amplitude *= 0.5;
    frequency *= 2.0;
    j++;
  }
  turbulence /= fs_max;
  float min_value = self->private.mMin;
  float max_value = self->private.mMax;
  float value = 0.0;
  value = turbulence;
  value = min_value + (max_value - min_value) * value;
  return (turbulence);
}

static float	noiseApplyfBm(NoiseClass *self, Point3D *hitPoint)
{
  float amplitude = 1.0;
  float frequency = 2.0;
  float fBm = 0.0;
  int	j;

  j = 0;
  while (j < self->private.mNumOctaves)
  {
    fBm += amplitude * pnoise(hitPoint->x * frequency,
			      hitPoint->y * frequency,
			      hitPoint->z * frequency);
    amplitude *= self->private.mGain;
    frequency *= self->private.mLacunarity;
    j++;
  }
  fBm = (fBm - self->private.mfBmMin)
      / (self->private.mfBmMax - self->private.mfBmMin);
  return (fBm);
}

static float	noiseApplyCosY(NoiseClass *self, Point3D *hitPoint)
{
  float fBm;

  fBm = 0.0;
  fBm += cos (hitPoint->y + pnoise(hitPoint->z * self->private.mGain,
				   hitPoint->y * self->private.mGain,
				   hitPoint->z * self->private.mGain));
  fBm = (fBm - self->private.mfBmMin)
      / (self->private.mfBmMax - self->private.mfBmMin);
  return (fBm);
}

static float	noiseApplyCosX(NoiseClass *self, Point3D *hitPoint)
{
  float fBm;

  fBm = 0.0;
  fBm += cos (hitPoint->x + pnoise(hitPoint->z * self->private.mGain,
				   hitPoint->y * self->private.mGain,
				   hitPoint->z * self->private.mGain));
  fBm = (fBm - self->private.mfBmMin)
      / (self->private.mfBmMax - self->private.mfBmMin);
  return (fBm);
}

static float	noiseGetColor(NoiseClass *self, HitRecordStruct *hitRecord)
{
  float	value;
  float min_value = self->private.mMin;
  float max_value = self->private.mMax;

  value = self->applyNoise(self, &hitRecord->hitPoint);
  value = min_value + (max_value - min_value) * value;
  return (value);
}

static void	noiseComputefBmBounds(NoiseClass *self)
{
  if (self->private.mGain == 1.0)
    self->private.mfsMax = self->private.mfBmMax = self->private.mNumOctaves;
  else
  {
    self->private.mfsMax = self->private.mfBmMax =
      (1.0 - pow(self->private.mGain,
		 self->private.mNumOctaves))
      / (1.0 - self->private.mGain);
    self->private.mfsMin = self->private.mfBmMin = -self->private.mfBmMax;
  }
}

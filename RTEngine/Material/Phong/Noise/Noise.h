#ifndef __NOISE_H__
# define __NOISE_H__

/* # include "Matte.h" */
# include "Phong.h"
# include "PerfectSpecular.h"
# include "SceneSerializer.h"

# define NOISE(x)		((NoiseClass*) (x))

typedef struct NoiseUnserializeStruct	NoiseUnserializeStruct;
struct					NoiseUnserializeStruct
{
  char			*strKr;
  char			*strCr;
  char			*strNoise;
  char			*strOctaves;
  char			*strGain;
  char			*strMax;
  char			*strMin;
  char			*strLacunarity;
  char			*strExp;
  float			kr;
  float			cr;
  float			octaves;
  float			lacunarity;
  float			gain;
  float			max;
  float			min;
  float			exp;
};

typedef struct NoiseClass	NoiseClass;
struct                          NoiseClass
{
  /* SuperClass : */
  PhongClass			__base__;
  /* public : */
  /* public : */
  void				(*setKr)(NoiseClass *self, float kr);
  void				(*setCr)(NoiseClass *self, float cr);
  void				(*setNoiseType)(NoiseClass *self,
						char *strNoiseType);
  float				(*getColor)(NoiseClass *self,
					    HitRecordStruct *hitRecord);
  void				(*computefBmBounds)(NoiseClass *self);
  float				(*applyNoise)(NoiseClass *self,
					      Point3D *hitPoint);
  /* private : */
  struct
  {
    PerfectSpecularClass	*mPerfectSpecular;
    float			mNumOctaves;
    float			mGain;
    float			mLacunarity;
    float			mMax;
    float			mMin;
    float			mfBmMax;
    float			mfBmMin;
    float			mfsMax;
    float			mfsMin;
    float			exp;
  } private;
};

extern Class*			Noise;

void	*noiseUnserialize(SceneSerializerClass *serializer,
                           xmlNodePtr node);

#endif /* !__NOISE_H__ */

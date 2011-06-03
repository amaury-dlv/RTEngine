#include <math.h>

#include "Maths.h"
#include "MultiJittered.h"
#include "Utils.h"

static void	multiJitteredConstructor();
static void	multiJitteredDestructor(MultiJitteredClass *self);
static void	multiJitteredGenerateSamples(SamplerClass *self);
static void	multiJitteredGenerateSample(SamplerClass *self,
                                            int p, int i, int j);

static MultiJitteredClass		_descr = { /* Sphere */
  { /* Sampler */
    { /* Class */
      sizeof(MultiJitteredClass),
      __RTTI_MULTIJITTERED,
      &multiJitteredConstructor,
      &multiJitteredDestructor,
    },
    /* public */
    0, /* generateSamples */
    0, /* getNumSamples */
    0, /* getNumSets */
    0, /* getE */
    &multiJitteredGenerateSample, /* generateSample (virtual) */
    0, /* setupShuffledIndices */
    0, /* nextSample (NE1) */
    0, /* nextHemisphereSample */
    0, /* nextSphereSample*/
    0, /* mapSamplesToUnitDisk */
    0, /* mapSamplesToHemisphere */
    0, /* mapSamplesToSphere */
    0, /* shuffleX */
    0, /* shuffleY */
    /* public */
    /* protected */
    {
      0, /* mNumSamples */
      0, /* mSqrtNumSamples */
      0, /* mNumSets */
      0, /* mSamples */
      0, /* mShuffledIndices */
      0, /* mDiskSamples */
      0, /* mHemisphereSamples */
      0, /* mSphereSamples */
      0, /* mCount */
      0, /* mJump */
      0, /* mE */
      {0, 0, 0, 0,
       0, 0, 0, 0,
       0, 0, 0, 0,
       0, 0, 0, 0}
    }
  },
  {
    0 /* mSubcellWidth */
  }
};

Class *MultiJittered = (Class*) &_descr;

static void	multiJitteredConstructor(MultiJitteredClass *self,
                                         va_list *list)
{
  printf("* Providing a multi-jittered sampler");
  Sampler->__constructor__(self, list);
  SAMPLER(self)->generateSamples = &multiJitteredGenerateSamples;
  SAMPLER(self)->setupShuffledIndices(SAMPLER(self));
  SAMPLER(self)->generateSamples(SAMPLER(self));
  SAMPLER(self)->shuffleX(SAMPLER(self));
  SAMPLER(self)->shuffleY(SAMPLER(self));
}

static void	multiJitteredDestructor(MultiJitteredClass *self)
{
}

static void	multiJitteredGenerateSamples(SamplerClass *self)
{
  int		p, i, j;

  printf(" Generating 2D samples\n");
  MULTIJITTERED(self)\
    ->private.mSubcellWidth = 1. / ((float) PROT(mNumSamples));
  p = 0;
  while (p < PROT(mNumSets))
  {
    i = 0;
    while (i < PROT(mSqrtNumSamples))
    {
      j = 0;
      while (j < PROT(mSqrtNumSamples))
      {
        self->generateSample(self, p, i, j);
        j++;
      }
      i++;
    }
    p++;
  }
}

static void	multiJitteredGenerateSample(SamplerClass *self,
                                            int p, int i, int j)
{
  PROT(mSamples)[i * PROT(mSqrtNumSamples) + j + p * PROT(mNumSamples)].x =
    (i * PROT(mSqrtNumSamples) + j)
    * MULTIJITTERED(self)->private.mSubcellWidth
    + rand_float2(0, MULTIJITTERED(self)->private.mSubcellWidth);
  PROT(mSamples)[i * PROT(mSqrtNumSamples) + j + p * PROT(mNumSamples)].y =
    (j * PROT(mSqrtNumSamples) + i)
    * MULTIJITTERED(self)->private.mSubcellWidth
    + rand_float2(0, MULTIJITTERED(self)->private.mSubcellWidth);
}

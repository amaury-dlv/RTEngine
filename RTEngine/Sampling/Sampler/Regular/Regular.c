#include <math.h>

#include "Maths.h"
#include "Regular.h"
#include "Utils.h"

static void	regularConstructor(RegularClass* self, va_list* list);
static void	regularDestructor(RegularClass* self);
static void	regularGenerateSample(SamplerClass* self, int count, int i, int j);

static RegularClass		_descr = { /* Sphere */
  { /* Sampler */
    { /* Class */
      sizeof(RegularClass),
      __RTTI_REGULAR, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
      &regularConstructor,
      &regularDestructor,
    },
    /* public */
    0, /* generateSamples */
    0, /* getNumSamples */
    0, /* getNumSets */
    0, /* getE */
    &regularGenerateSample, /* generateSample (virtual) */
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
       0, 0, 0, 0} /* mThread */
    }
  }
};

Class* Regular = (Class*) &_descr;

static void	regularConstructor(RegularClass* self, va_list* list)
{
  printf("* Providing a regular sampler");
  Sampler->__constructor__(self, list);
  SAMPLER(self)->setupShuffledIndices(SAMPLER(self));
  SAMPLER(self)->generateSamples(SAMPLER(self));
}

static void	regularDestructor(RegularClass* self)
{
}

static void	regularGenerateSample(SamplerClass* self, int count, int i, int j)
{
  self->protected.mSamples[count] =
    (Point2D){(i + 0.5) / self->protected.mSqrtNumSamples,
              (j + 0.5) / self->protected.mSqrtNumSamples};
}

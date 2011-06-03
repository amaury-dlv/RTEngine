#include <math.h>

#include "Maths.h"
#include "PureRandom.h"
#include "Utils.h"

static void	pureRandomConstructor(PureRandomClass *self, va_list *list);
static void	pureRandomDestructor(PureRandomClass *self);
static void	pureRandomGenerateSample(SamplerClass *self, int count, int i, int j);

static PureRandomClass		_descr = { /* Sphere */
  { /* Sampler */
    { /* Class */
      sizeof(PureRandomClass),
      __RTTI_PURERANDOM, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
      &pureRandomConstructor,
      &pureRandomDestructor,
    },
    /* public */
    0, /* generateSamples */
    0, /* getNumSamples */
    0, /* getNumSets */
    0, /* getE */
    &pureRandomGenerateSample, /* generateSample (virtual) */
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

Class *PureRandom = (Class*) &_descr;

static void	pureRandomConstructor(PureRandomClass *self, va_list *list)
{
  printf("* Providing a pure random sampler");
  Sampler->__constructor__(self, list);
  SAMPLER(self)->setupShuffledIndices(SAMPLER(self));
  SAMPLER(self)->generateSamples(SAMPLER(self));
}

static void	pureRandomDestructor(PureRandomClass *self)
{
}

static void	pureRandomGenerateSample(SamplerClass *self, int count, int i, int j)
{
  self->protected.mSamples[count] =
    (Point2D){rand_float(), rand_float()};
}

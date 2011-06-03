#include <math.h>

#include "Maths.h"
#include "Jittered.h"
#include "Utils.h"

static void	jitteredConstructor(JitteredClass *self, va_list *list);
static void	jitteredDestructor(JitteredClass *self);
static void	jitteredGenerateSample(SamplerClass *self,
                                       int count, int i, int j);

static JitteredClass		_descr = { /* Sphere */
  { /* Sampler */
    { /* Class */
      sizeof(JitteredClass),
      __RTTI_JITTERED, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
      &jitteredConstructor,
      &jitteredDestructor,
    },
    /* public */
    0, /* generateSamples */
    0, /* getNumSamples */
    0, /* getNumSets */
    0, /* getE */
    &jitteredGenerateSample, /* generateSample (virtual) */
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

Class *Jittered = (Class*) &_descr;

static void	jitteredConstructor(JitteredClass *self, va_list *list)
{
  printf("* Providing a jittered sampler");
  Sampler->__constructor__(self, list);
  SAMPLER(self)->setupShuffledIndices(SAMPLER(self));
  SAMPLER(self)->generateSamples(SAMPLER(self));
}

static void	jitteredDestructor(JitteredClass *self)
{
}

static void	jitteredGenerateSample(SamplerClass *self,
                                       int count, int i, int j)
{
  self->protected.mSamples[count] =
    (Point2D){((float)i + rand_float())
      / (float)self->protected.mSqrtNumSamples,
      ((float)j + rand_float())
        / (float)self->protected.mSqrtNumSamples};
}

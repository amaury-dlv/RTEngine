#include <stdio.h>
#include <math.h>

#include "my_omp.h"
#include "Sampler.h"
#include "SamplerManager.h"
#include "Utils.h"
#include "Maths.h"
#include "Point2D.h"
#include "Point3D.h"
#include "Vector3D.h"

static void	samplerConstructor(SamplerClass *self, va_list *list);
static void	samplerDestructor(SamplerClass *self, va_list *list);
static int	samplerGetNumSamples(SamplerClass *self);
static int	samplerGetNumSets(SamplerClass *self);
static float	samplerGetE(SamplerClass *self);
static void	samplerSetupShuffledIndices(SamplerClass *self);
static void	samplerShuffleX(SamplerClass *self);
static void	samplerShuffleY(SamplerClass *self);
static void	samplerGenerateSamples(SamplerClass *self);
static float	samplerCaseOne(Point2D sp, float phi, float *r);
static float	samplerCaseTwo(Point2D sp, float phi, float *r);
static void	samplerMapSamplesToUnitDisk(SamplerClass *self);
static void	samplerMapSamplesToHemisphere(SamplerClass *self, float e);
static void	samplerMapSamplesToSphere(SamplerClass *self);
static int	samplerNextSampleIndex(SamplerClass *self);
static Point2D	samplerNextSample(SamplerClass *self);
static Vector3D	samplerNextSphereSample(SamplerClass *self);
static Vector3D	samplerNextHemisphereSample(SamplerClass *self);

static SamplerClass       _descr = { /* SamplerClass */
    { /* Class */
      sizeof(SamplerClass),
      0, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
      &samplerConstructor,
      &samplerDestructor,
    },
    /* public */
    0, /* generateSamples */
    0, /* getNumSamples */
    0, /* getNumSets */
    0, /* getE */
    0, /* generateSample (virtual) */
    0, /* setupShuffledIndices */
    0, /* nextSample */
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
  };

/*
** Never new it !!!
*/
Class *Sampler = (Class*) &_descr;

static void	samplerConstructor(SamplerClass *self, va_list *list)
{
  self->protected.mNumSamples = va_arg(*list, int);
  self->protected.mNumSets = va_arg(*list, int);
  self->protected.mE = (float) va_arg(*list, double);
  self->protected.mSqrtNumSamples = sqrt(self->protected.mNumSamples);
  self->generateSamples = &samplerGenerateSamples;
  self->getNumSamples = &samplerGetNumSamples;
  self->getNumSets = &samplerGetNumSets;
  self->getE = &samplerGetE;
  self->setupShuffledIndices = &samplerSetupShuffledIndices;
  self->nextSample = &samplerNextSample;
  self->nextHemisphereSample = &samplerNextHemisphereSample;
  self->nextSphereSample = &samplerNextSphereSample;
  self->mapSamplesToUnitDisk = &samplerMapSamplesToUnitDisk;
  self->mapSamplesToSphere = &samplerMapSamplesToSphere;
  samplerInit(self, list);
}


static void	samplerDestructor(SamplerClass *self, va_list *list)
{
}

void		samplerInit(SamplerClass *self, va_list *list)
{
  self->mapSamplesToHemisphere = &samplerMapSamplesToHemisphere;
  self->shuffleX = &samplerShuffleX;
  self->shuffleY = &samplerShuffleY;
  self->protected.mSamples =
    xmalloc(sizeof(Point2D) * self->protected.mNumSamples
                            * self->protected.mNumSets);
  self->protected.mDiskSamples =
    xmalloc(sizeof(Point2D) * self->protected.mNumSamples);
  self->protected.mHemisphereSamples =
    xmalloc(sizeof(Vector3D) * self->protected.mNumSamples
	                           * self->protected.mNumSets);
  self->protected.mSphereSamples =
    xmalloc(sizeof(Vector3D) * self->protected.mNumSamples
	                           * self->protected.mNumSets);
  printf(" with %d sets of %d samples\n",
      self->protected.mNumSets,
      self->protected.mNumSamples);
}

static int	samplerGetNumSamples(SamplerClass *self)
{
  return (self->protected.mNumSamples);
}

static int	samplerGetNumSets(SamplerClass *self)
{
  return (self->protected.mNumSets);
}

static float	samplerGetE(SamplerClass *self)
{
  return (self->protected.mE);
}

static void	samplerSetupShuffledIndices(SamplerClass *self)
{
  int		*indices;
  int		i, j;
  int	 	is = 0;

  PROT(mShuffledIndices) = xmalloc(sizeof(*PROT(mShuffledIndices))
        * PROT(mNumSamples) * PROT(mNumSets));
  indices = xmalloc(sizeof(*indices) * self->protected.mNumSamples);
  i = -1;
  while (++i < self->protected.mNumSamples)
    indices[i] = i;
  i = -1;
  while (++i < self->protected.mNumSets)
  {
    randomShuffle(indices, PROT(mNumSamples));
    j = -1;
    while (++j < PROT(mNumSamples))
      PROT(mShuffledIndices)[is++] = indices[j];
  }
  free(indices);
}

static void	samplerShuffleX(SamplerClass *self)
{
  int		p;
  int		i;

  p = 0;
  while (p < PROT(mNumSets))
  {
    i = 0;
    while (i <  PROT(mNumSamples) - 1)
    {
      int target = rand_int() % PROT(mNumSamples)
        + p * PROT(mNumSamples);
      float temp = PROT(mSamples)[i + p * PROT(mNumSamples) + 1].x;
      PROT(mSamples)[i + p * PROT(mNumSamples) + 1].x =
        PROT(mSamples)[target].x;
      PROT(mSamples)[target].x = temp;
      i++;
    }
    p++;
  }
}

static void	samplerShuffleY(SamplerClass *self)
{
  int		p;
  int		i;

  p = 0;
  while (p < PROT(mNumSets))
  {
    i = 0;
    while (i <  PROT(mNumSamples) - 1)
    {
      int target = rand_int() % PROT(mNumSamples)
        + p * PROT(mNumSamples);
      float temp = PROT(mSamples)[i + p * PROT(mNumSamples) + 1].y;
      PROT(mSamples)[i + p * PROT(mNumSamples) + 1].y =
        PROT(mSamples)[target].y;
      PROT(mSamples)[target].y = temp;
      i++;
    }
    p++;
  }
}

static void	samplerGenerateSamples(SamplerClass *self)
{
  int		set, i, j, count;
  float		e;

  printf(" Generating 2D samples\n");
  e = 0;
  count = 0;
  set = 0;
  while (set < self->protected.mNumSets)
  {
    i = 0;
    while (i < self->protected.mSqrtNumSamples)
    {
      j = 0;
      while (j < self->protected.mSqrtNumSamples)
      {
        self->generateSample(self, count++, i, j);
        j++;
      }
      i++;
    }
    set++;
  }
  samplerMapSamplesToUnitDisk(self);
  samplerMapSamplesToHemisphere(self, self->protected.mE);
  samplerMapSamplesToSphere(self);
}

static float	samplerCaseOne(Point2D sp, float phi, float *r)
{
  if (sp.x > sp.y)
    {
      *r = sp.x;
      phi = sp.y / sp.x;
    }
  else
    {
      *r = sp.y;
      phi = 2. - sp.x / sp.y;
    }
  return (phi);
}

static float	samplerCaseTwo(Point2D sp, float phi, float *r)
{
  if (sp.x < sp.y)
    {
      *r = -sp.x;
      phi = 4. + sp.y / sp.x;
    }
  else
    {
      *r = -sp.y;
      if (sp.y != 0.)
	phi = 6. - sp.x / sp.y;
      else
	phi = 0.;
    }
  return (phi);
}

static void	samplerMapSamplesToUnitDisk(SamplerClass *self)
{
  int		size;
  int		i;
  float		r;
  float		phi;
  Point2D	sp;

  printf(" Generating unit disk samples\n");
  size = self->protected.mNumSamples;
  i = 0;
  while (i < size)
    {
      sp.x = 2. * self->protected.mSamples[i].x - 1.;
      sp.y = 2. * self->protected.mSamples[i].y - 1.;
      if (sp.x > -sp.y)
	phi = samplerCaseOne(sp, phi, &r);
      else
	phi = samplerCaseTwo(sp, phi, &r);
      phi *= Pi / 4.;
      self->protected.mDiskSamples[i].x = r * cos(phi);
      self->protected.mDiskSamples[i].y = r * sin(phi);
      i++;
    }
}

static void	samplerMapSamplesToHemisphere(SamplerClass *self,
                                              float e)
{
  int		size;
  int		i;
  float		cos_phi;
  float		sin_phi;
  float		cos_theta;
  float		sin_theta;

  printf(" Generating hemisphere samples (e: %f)\n", e);
  size = self->protected.mNumSamples * self->protected.mNumSets;
  i = 0;
  while (i < size)
    {
      cos_phi = cos(2. * Pi * self->protected.mSamples[i].x);
      sin_phi = sin(2. * Pi * self->protected.mSamples[i].x);
      cos_theta = pow((1. - self->protected.mSamples[i].y), (1. / (e + 1.)));
      sin_theta = sqrt(1. - cos_theta * cos_theta);
      self->protected.mHemisphereSamples[i].x = sin_theta * cos_phi;
      self->protected.mHemisphereSamples[i].y = sin_theta * sin_phi;
      self->protected.mHemisphereSamples[i].z = cos_theta;
      i++;
    }
}

static void	samplerMapSamplesToSphere(SamplerClass *self)
{
  int		size;
  int		i;
  float		r;
  float		phi;

  printf(" Generating sphere samples\n");
  size = self->protected.mNumSamples * self->protected.mNumSets;
  i = 0;
  while (i < size)
    {
      self->protected.mSphereSamples[i].z =
        1. - 2. * self->protected.mSamples[i].x;
      r = sqrt(1. - self->protected.mSphereSamples[i].z
          * self->protected.mSphereSamples[i].z);
      phi = TwoPi * self->protected.mSamples[i].y;
      self->protected.mSphereSamples[i].x = r * cos(phi);
      self->protected.mSphereSamples[i].y = r * sin(phi);
      i++;
    }
}

static int	samplerNextSampleIndex(SamplerClass *self)
{
  int			i;
#ifdef USE_THREADS
  int			curThread;

  curThread = omp_get_thread_num();
  if (PROT(mThread)[curThread] % PROT(mNumSamples) == 0)
    PROT(mJump) = (rand() % PROT(mNumSets)) * PROT(mNumSamples);
  i = PROT(mJump)
    + PROT(mShuffledIndices)[PROT(mJump)
    + (PROT(mThread)[curThread]++ % PROT(mNumSamples))];
#else
  if (self->protected.mCount % self->protected.mNumSamples == 0)
    self->protected.mJump = (rand() % self->protected.mNumSets)
      * self->protected.mNumSamples;
  i = self->protected.mJump
    + (self->protected.mCount++ % self->protected.mNumSamples);
#endif
  return (i);
}

static Point2D		samplerNextSample(SamplerClass *self)
{
  int		i;

  i = samplerNextSampleIndex(self);
  return (self->protected.mSamples[i]);
}

static Vector3D	samplerNextSphereSample(SamplerClass *self)
{
  int		i;

  i = samplerNextSampleIndex(self);
  return (self->protected.mSphereSamples[i]);
}

static Vector3D	samplerNextHemisphereSample(SamplerClass *self)
{
  int		i;

  i = samplerNextSampleIndex(self);
  return (self->protected.mHemisphereSamples[i]);
}

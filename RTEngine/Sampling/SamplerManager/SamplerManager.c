#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "SamplerManager.h"
#include "RTEngine.h"

static void     samplerManagerConstructor();
static void     samplerManagerDestructor();
static SamplerClass	*samplerManagerAddSampler(SamplerType type,
                                                  int numSamples,
                                                  int numSets,
                                                  float e);
static SamplerClass*	samplerManagerGetSampler(SamplerType type,
                                                 int numSamples,
                                                 int numSets,
                                                 float e);

/*
 ** The static binding of the functions from the NormeExporter cannot
 ** be done at compile time : "initializer element is not constant".
 ** It has to be done in the contructor.
 */
static SamplerManagerClass       _descr = { /* SamplerManagerClass */
    { /* Class */
      sizeof(SamplerManagerClass),
      0, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
      &samplerManagerConstructor,
      &samplerManagerDestructor,
    },
    &samplerManagerGetSampler,
    {
      0, /* mSamplerVect */
    }
  };

Class* SamplerManager = (Class*) &_descr;

static SamplerManagerClass* sSingleton = 0;

static void     samplerManagerConstructor(SamplerManagerClass* self,
					  va_list* list)
{
  if (sSingleton != 0)
    raise("Cannot new a singleton twice : SamplerManager\n");
  sSingleton = self;
  srand(time(NULL));
  self->private.mSamplerVect = new(Vector);
}

static void     samplerManagerDestructor(SamplerManagerClass *self)
{
  int		i;

  i = 0;
  while (i < self->private.mSamplerVect\
      ->size(self->private.mSamplerVect))
  {
    delete(self->private.mSamplerVect->mPtrs[i]);
    i++;
  }
  delete(self->private.mSamplerVect);
}

SamplerManagerClass*	SamplerManagerGetSingletonPtr(void)
{
  return (sSingleton);
}

static SamplerClass	*samplerManagerAddSampler(SamplerType type,
                                                  int numSamples,
                                                  int numSets,
                                                  float e)
{
  SamplerManagerClass	*this;
  SamplerClass		*sampler;

  this = SamplerManagerGetSingletonPtr();
  sampler = NULL;
  if (type == eRegular)
    sampler = new(Regular, numSamples, numSets, e);
  else if (type == ePureRandom)
    sampler = new(PureRandom, numSamples, numSets, e);
  else if (type == eJittered)
    sampler = new(Jittered, numSamples, numSets, e);
  else if (type == eMultiJittered)
    sampler = new(MultiJittered, numSamples, numSets, e);
  else
    raise("No such sampler.\n");
  this->private.mSamplerVect\
    ->push_back(this->private.mSamplerVect, sampler);
  return (sampler);
}

static SamplerClass*	samplerManagerGetSampler(SamplerType type,
                                                 int numSamples,
                                                 int numSets,
                                                 float e)
{
  int			i;
  va_list		list;
  SamplerClass		*cur;
  SamplerManagerClass	*this;

  i = 0;
  this = SamplerManagerGetSingletonPtr();
  if ((int)sqrtf(numSamples) * (int)sqrtf(numSamples) != (float)numSamples)
    raise("numSamples must be sqrtizable\n");
  if (RTEngineGetSingletonPtr()->getSamplerShared())
    while (i < this->private.mSamplerVect->size(this->private.mSamplerVect))
    {
      cur = this->private.mSamplerVect->mPtrs[i];
      if (cur->getNumSamples(cur) == numSamples
          && cur->getNumSets(cur) == numSets
          && cur->getE(cur) == e)
      {
        va_end(list);
        return (cur);
      }
      i++;
    }
  cur = samplerManagerAddSampler(type, numSamples, numSets, e);
  return (cur);
}

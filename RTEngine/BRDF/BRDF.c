#include "BRDF.h"
#include "RTEngine.h"

static void     BRDFConstructor(BRDFClass *self, va_list *list);
static void     BRDFDestructor(BRDFClass *self);

static BRDFClass       _descr = { /* BRDFClass */
  { /* Class */
    sizeof(BRDFClass),
    0, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
    &BRDFConstructor,
    &BRDFDestructor,
  },
  0, /* f() */
  0, /* sampleF() */
  0, /* sampleFPath() */
  0, /* rho() */
  0,
  { /* protected */
    0 /* mSamplerType */
  }
};

Class *BRDF = (Class*) &_descr;

static void     BRDFConstructor(BRDFClass *self, va_list *list)
{
  self->protected.mSampler = SamplerManagerGetSingletonPtr()\
      ->getSampler(eMultiJittered,
        RTEngineGetSingletonPtr()->getDefaultNumSamples(),
	RTEngineGetSingletonPtr()->getDefaultNumSets(), (float)100.);
  self->protected.mSampler->mapSamplesToHemisphere(self->protected.mSampler,
                                                   (float)1.);
}

static void     BRDFDestructor(BRDFClass *self)
{
}

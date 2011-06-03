#ifndef __PERFECTTRANSMITTER_H__
# define __PERFECTTRANSMITTER_H__

# include "BTDF.h"

# define PERFECTTRANS(x)	((PerfectTransmitterClass*) (x))

typedef struct PerfectTransmitterClass	PerfectTransmitterClass;
struct                          	PerfectTransmitterClass
{
  /* SuperClass : */
  BTDFClass			__base__;
  /* public : */
  void				(*setKt)(PerfectTransmitterClass *self, float kt);
  void				(*setIor)(PerfectTransmitterClass *self, float ior);
  /* protected : */
  struct
  {
    float			mKt;
    float			mIor;
  } private;
};

extern Class*			PerfectTransmitter;

#endif /* !__PERFECTTRANSMITTER_H__ */

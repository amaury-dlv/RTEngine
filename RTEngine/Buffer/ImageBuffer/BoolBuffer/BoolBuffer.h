#ifndef __BOOLBUFFER_H__
# define __BOOLBUFFER_H__

# include "ImageBuffer.h"
# include "HitRecord.h"

# define	BOOLBUFFER(x)	((BoolBufferClass *)x)

# define	GET(x)		(!!(PRIV(mBuffer)[(x) / 8] & (1 << ((x) % 8))))

# define	SET(x)		(PRIV(mBuffer)[(x) / 8] |= (1 << ((x) % 8)))
# define	UNSET(x)	(PRIV(mBuffer)[(x) / 8] &= ~(1 << ((x) % 8)))

typedef struct BoolBufferClass	BoolBufferClass;
struct                          BoolBufferClass
{
  /* SuperClass : */
  ImageBufferClass		__base__;
  /* public : */
  bool				(*get)(BoolBufferClass *self, int i);
  void				(*set)(BoolBufferClass *self, int x, int y, bool bit);
  /* private : */
  struct
  {
    unsigned char		*mBuffer;
  } private;
};

extern Class*			BoolBuffer;

#endif /* !__BOOLBUFFER_H__ */

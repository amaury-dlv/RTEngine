#ifndef __VECTOR_H__
# define __VECTOR_H__

# include "OOC.h"

typedef struct VectorClass	VectorClass;
struct                          VectorClass
{
  /* SuperClass : */
  Class				__base__;
  /* public : */
  void				(*push_back)(VectorClass *self, void *ptr);
  void				(*pop_back)(VectorClass *self);
  void				(*reserve)(VectorClass *self, int batch);
  int				(*size)(VectorClass *self);
  /* public : */
  void**			mPtrs;
  /* private : */
  struct
  {
    int				mSize;
    int				mBatchAllocation;
  } private;
};

extern Class*			Vector;

#endif /* !__VECTOR_H__ */

#ifndef __DEQUEUE_H__
# define __DEQUEUE_H__

# include "OOC.h"

typedef struct DequeueClass	DequeueClass;
struct                          DequeueClass
{
  /* SuperClass : */
  Class				__base__;
  /* public : */
  void				(*push_back)(DequeueClass *self, void *ptr);
  void				(*push_front)(DequeueClass *self, void *ptr);
  void*				(*pop_back)(DequeueClass *self);
  void*				(*pop_front)(DequeueClass *self);
  void*				(*front)(DequeueClass *self);
  void*				(*back)(DequeueClass *self);
  void				(*clear)(DequeueClass *self);
  void				(*reserve)(DequeueClass *self, int batch);
  int				(*size)(DequeueClass *self);
  /* public : */
  void**			mPtrs;
  /* private : */
  struct
  {
    int				mSize;
    int				mBatchAllocation; /* -> reverve? */
  } private;
};

extern Class*			Dequeue;

#endif /* !__DEQUEUE_H__ */

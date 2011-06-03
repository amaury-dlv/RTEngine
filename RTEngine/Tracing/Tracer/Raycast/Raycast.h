#ifndef __RAYCAST_H__
# define __RAYCAST_H__

# include "Tracer.h"
# include "HitRecord.h"

# define 	RAYCAST(x)	((RaycastClass*) (x))

typedef struct RaycastClass	RaycastClass;
struct                          RaycastClass
{
  /* SuperClass : */
  TracerClass			__base__;
  /* public : */
  HitRecordStruct*		(*hit)(RaycastClass *self, RayStruct *ray,
                                       HitRecordStruct *hitRecord);
};

extern Class*			Raycast;

#endif /* !__RAYCAST_H__ */

#ifndef __HITRECORD_H__
# define __HITRECORD_H__

# include "OOC.h"
# include "Scene.h"
# include "Vector3D.h"
# include "Ray.h"
# include "Point3D.h"
# include "Color.h"
# include "Material.h"

typedef struct HitRecordStruct	HitRecordStruct;
struct				HitRecordStruct
{
  double			distLight;
  Vector3D			normal;
  Point3D			hitPoint;
  Point3D			localHitPoint;
  RayStruct			ray;
  Color				color;
  MaterialClass			*material;
  int				reflectDepth;
  bool				hit;
  bool				inside;
};

void		hitRecordReset(HitRecordStruct *self);
double		hitRecordGetDepth(HitRecordStruct *self);
HitRecordStruct	*hitRecordCopy(HitRecordStruct *hitRecord);
void		buildHitRecordFromGBufferWithLight(HitRecordStruct *hitRecord,
                                           LightClass *light, int i);

#endif /* !__HITRECORD_H__ */

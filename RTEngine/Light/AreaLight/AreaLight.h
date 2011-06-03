#ifndef __AREALIGHT_H__
# define __AREALIGHT_H__

# include "Light.h"
# include "Point3D.h"
# include "HitRecord.h"

# define AREALIGHT(x)		((AreaLightClass*) (x))

typedef struct AreaLightClass	AreaLightClass;
struct                          AreaLightClass
{
  /* SuperClass : */
  LightClass			__base__;
  /* public: */
  float				(*g)(AreaLightClass *self,
                                     HitRecordStruct *hitRecord);
  float				(*pdf)(AreaLightClass *self,
                                       HitRecordStruct *hitRecord);
  int				samples;
  /* private: */
  struct
  {
    GeometricObjectClass	*mObject;
    MaterialClass		*mMaterial;
    Point3D			samplePoint;
    Point3D			samplePoint1[16];
    Vector3D			normal;
    Vector3D			wi;
  } private;
};

extern Class*			AreaLight;

#endif /* !__AREALIGHT_H__ */

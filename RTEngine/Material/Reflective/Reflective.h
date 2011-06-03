#ifndef __REFLECTIVE_H__
# define __REFLECTIVE_H__

# include "Phong.h"
# include "PerfectSpecular.h"
# include "SceneSerializer.h"

# define REFLECTIVE(x)		((ReflectiveClass*) (x))

typedef struct ReflectiveClass	ReflectiveClass;
struct                          ReflectiveClass
{
  /* SuperClass : */
  PhongClass			__base__;
  /* public : */
  /* public : */
  void				(*setKr)(ReflectiveClass *self, float kr);
  void				(*setCr)(ReflectiveClass *self, float cr);
  /* private : */
  struct
  {
    PerfectSpecularClass	*mPerfectSpecular;
  } private;
};

extern Class*			Reflective;

void	*reflectiveUnserialize(SceneSerializerClass *serializer,
                           xmlNodePtr node);

#endif /* !__REFLECTIVE_H__ */

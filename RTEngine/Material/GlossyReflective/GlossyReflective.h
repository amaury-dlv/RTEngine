#ifndef __GLOSSYREFLECTIVE_H__
# define __GLOSSYREFLECTIVE_H__

# include "Phong.h"
# include "GlossySpecular.h"
# include "SceneSerializer.h"

# define GLREFLECTIVE(x)	((GlossyReflectiveClass*) (x))

typedef struct GlossyReflectiveClass	GlossyReflectiveClass;
struct                          GlossyReflectiveClass
{
  /* SuperClass : */
  PhongClass			__base__;
  /* public : */
  /* public : */
  void				(*setCr)(GlossyReflectiveClass *self, float cr);
  /* private : */
};

extern Class*			GlossyReflective;

void	*glossyReflectiveUnserialize(SceneSerializerClass *serializer,
                           xmlNodePtr node);

#endif /* !__GLOSSYREFLECTIVE_H__ */

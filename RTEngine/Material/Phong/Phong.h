#ifndef __PHONG_H__
# define __PHONG_H__

# include "Material.h"
# include "SceneSerializer.h"

# define PHONG(x)		((PhongClass*) (x))

typedef struct PhongClass	PhongClass;
struct                          PhongClass
{
  /* SuperClass : */
  MaterialClass			__base__;
  /* public : */
  /* public : */
  void				(*setKd)(PhongClass *self, float kd);
  void				(*setKs)(PhongClass *self, float ks);
  void				(*setExp)(PhongClass *self, float exp);
  /* private : */
  struct
  {
    void			*mDiffuseBrdf;
    void			*mGlossySpecularBrdf;
  } private;
};

extern Class*			Phong;

void	*phongUnserialize(SceneSerializerClass *serializer,
                           xmlNodePtr node);
MaterialIdAssoc	*phongUnserializeSet(SceneSerializerClass *serializer,
                           xmlNodePtr node,
                           PhongClass *self);

#endif /* !__PHONG_H__ */

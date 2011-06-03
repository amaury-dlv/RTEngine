#ifndef __BUMP_H__
# define __BUMP_H__

# include "Phong.h"
# include "Transparent.h"
# include "PerfectSpecular.h"
# include "SceneSerializer.h"

# define BUMP(x)		((BumpClass*) (x))

# define FREQ01			0.1
# define FREQ02			0.3
# define FREQ03			0.2
# define EPS01			0.56

typedef struct BumpClass	BumpClass;
struct                          BumpClass
{
  /* SuperClass : */
  TransparentClass		__base__;
  /* public : */
  /* public : */
  void				(*setBumpType)(BumpClass *self, char *strBump);
  void				(*bumpNormal)(BumpClass *self,
					      HitRecordStruct *HitRecordStruct,
					      Vector3D *normalPtr);
  /* private : */
  struct
  {
  } private;
};

extern Class*			Bump;

void	*bumpUnserialize(SceneSerializerClass *serializer,
                           xmlNodePtr node);

#endif /* !__BUMP_H__ */

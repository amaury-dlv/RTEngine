#ifndef __CHECKER_H__
# define __CHECKER_H__

/* # include "Matte.h" */
# include "Transparent.h"
# include "PerfectSpecular.h"
# include "SceneSerializer.h"

# define CHECKER(x)		((CheckerClass*) (x))

typedef struct CheckerClass	CheckerClass;
struct                          CheckerClass
{
  /* SuperClass : */
  TransparentClass		__base__;
  /* public : */
  /* public : */
  void				(*checker3D)(CheckerClass *self,
					     HitRecordStruct *hitRecord,
					     Color *color);
  /* private : */
  struct
  {
    Color			*mInsideColor;
    float			mSize;
  } private;
};

extern Class*			Checker;

void	*checkerUnserialize(SceneSerializerClass *serializer,
                           xmlNodePtr node);

#endif /* !__CHECKER_H__ */

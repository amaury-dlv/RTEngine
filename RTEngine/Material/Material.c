#include "Material.h"
#include "RenderingScene.h"
#include "SceneSerializer.h"

static void	materialConstructor(MaterialClass *self, va_list *list);
static void	materialDestructor(MaterialClass *self);

static MaterialClass       _descr = { /* MaterialClass */
    { /* Class */
      sizeof(MaterialClass),
      0, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
      &materialConstructor,
      &materialDestructor,
    },
    /* public */
    0, /* shade: virtual */
    0, /* shadeRGB: virtual */
    0, /* shadeAreaLight */
    0, /* shadePath */
    /* public */
    {0., 0., 0.}, /* color */
    /* protected */
    {
      0, /* normalBuffer */
      0, /* pointBuffer */
      0 /* rayBuffer */
    }
  };

/*
** Never new it !!!
*/
Class *Material = (Class*) &_descr;

static void	materialConstructor(MaterialClass *self, va_list *list)
{
}

static void	materialDestructor(MaterialClass *self)
{
}

int		materialUnserializeSet(SceneSerializerClass *serializer,
                                        xmlNodePtr node,
                                        MaterialClass *self)
{
  char		*strColor;

  serializer->setStrsFromProp(serializer, node, "color", &strColor, NULL);
  if (setColorFromStr(strColor, &self->color) != 0)
    return (-1);
  return (0);
}

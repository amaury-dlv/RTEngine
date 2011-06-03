#include "RTTI.h"
#include "Light.h"
#include "SceneSerializer.h"
#include "Utils.h"
#include "HitRecord.h"
#include "PointLight.h"

static void     lightConstructor(LightClass* self, va_list* list);
static void     lightDestructor(LightClass*self);
static void	lightSetHitRecordDist(LightClass *self,
                                      HitRecordStruct *rec);

static LightClass       _descr = { /* LightClass */
    { /* Class */
      sizeof(LightClass),
      0, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
      &lightConstructor,
      &lightDestructor,
    },
    0, /* getDirection(), virtual */
    0, /* l(), virtual */
    0, /* inShadow(), virtual */
    0, /* lightSetHitRecordDist */
    0., /* id */
    0., /* ls */
    {1, 1, 1}, /* color */
  };

Class* Light = (Class*) &_descr;

static int	mCurrentId = 0;

static void     lightConstructor(LightClass* self, va_list* list)
{
  self->id = mCurrentId++;
  self->setHitRecordDist = &lightSetHitRecordDist;
}

static void     lightDestructor(LightClass* self)
{
}

int		lightUnserializeSet(SceneSerializerClass *serializer,
                                    xmlNodePtr node,
                                    LightClass *self)
{
  char		*strLs;
  char		*strColor;

  serializer->setStrsFromProp(serializer, node,
      "radiance", &strLs, "color", &strColor, NULL);
  if (setFloatFromStr(strLs, &self->ls) != 0
      || setColorFromStr(strColor, &self->color) != 0)
  {
    fprintf(stderr, "Error: bad attributes\n");
    return (-1);
  }
  return (0);
}

static void	lightSetHitRecordDist(LightClass *self,
                                      HitRecordStruct *rec)
{
  if (RTTI(self) == RTTI(PointLight)
      || RTTI(self) == __RTTI_VPL)
    rec->distLight = vectDistXYZ(&(POINTLIGHT(self)->mPos),
                               rec->hitPoint.x,
                               rec->hitPoint.y,
                               rec->hitPoint.z);
  else
    rec->distLight = 1;
}

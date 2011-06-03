#include "Pinhole.h"
#include "SceneSerializer.h"
#include "Utils.h"

static void     pinholeConstructor(PinholeClass* self, va_list* list);
static void     pinholeDestructor();
static Vector3D	pinholeGetRayDir(PinholeClass* self, float x, float y);
static RayStruct	pinholeGetRay(CameraClass* self, float x, float y);
static void	pinholeSetViewDist(PinholeClass* self, float dist);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static PinholeClass       _descr = { /* PinholeClass */
  { /* CameraClass */
    { /* Class */
      sizeof(PinholeClass),
      0, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
      &pinholeConstructor,
      &pinholeDestructor,
    },
    0, /* setEye */
    0, /* setLookAt */
    0, /* setUp */
    &pinholeGetRay, /* getRay */
    0, /* computeUVW */
    {
      {0, 0, 0}, /* mEye */
      {0, 0, 0}, /* mLookAt */
      {0, 0, 0}, /* mUp */
      {0, 0, 0}, /* mU */
      {0, 0, 0}, /* mV */
      {0, 0, 0} /* mW */
    }
  },
  &pinholeSetViewDist,
  {
    50, /* mViewDist */
  }
};

Class* Pinhole = (Class*) &_descr;

static void     pinholeConstructor(PinholeClass* self,
                                             va_list* list)
{
  Camera->__constructor__(self, list);
}

static void     pinholeDestructor(PinholeClass* self)
{
  Camera->__destructor__(self);
}

void		*pinholeUnserialize(SceneSerializerClass *serializer,
                                    xmlNodePtr node)
{
  PinholeClass	*self;
  char		*strDis;

  self = new(Pinhole);
  if (cameraUnserializeSet(serializer, node, self) != 0)
    return (0);
  serializer->setStrsFromProp(serializer, node, "dis", &strDis, NULL);
  if (setFloatFromStr(strDis, &self->private.mViewDist) != 0)
  {
    fprintf(stderr, "Error: bad attributes\n");
    return (0);
  }
  return (self);
}

/* vvv */

/*
 * dir = x * u + y * v - d * w
 */
static Vector3D	pinholeGetRayDir(PinholeClass* self, float x, float y)
{
  Vector3D	u = CAMERA(self)->protected.mU;
  Vector3D	v = CAMERA(self)->protected.mV;
  Vector3D	w = CAMERA(self)->protected.mW;
  Vector3D	dir;

  vectOpTimes(&u, x);
  vectOpTimes(&v, y);
  vectOpTimes(&w, self->private.mViewDist);
  vectOpPlus(&u, &v);
  vectOpMinus(&u, &w);
  dir = u;
  vectNormalize(&dir);
  return (dir);
}

static RayStruct	pinholeGetRay(CameraClass* self, float x, float y)
{
  RayStruct	ray;

  ray.origin = self->protected.mEye;
  ray.direction = pinholeGetRayDir(PINHOLE(self), x, y);
  return (ray);
}

/* ^^^ */

static void		pinholeSetViewDist(PinholeClass* self, float dist)
{
  self->private.mViewDist = dist;
}

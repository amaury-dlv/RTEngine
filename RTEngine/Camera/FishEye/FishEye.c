#include "RTEngine.h"
#include "FishEye.h"
#include "SceneSerializer.h"
#include "Utils.h"
#include "Maths.h"
#include "Point2D.h"
#include "Maths.h"

static void     fishEyeConstructor(FishEyeClass *self, va_list *list);
static void     fishEyeDestructor();
static Vector3D	fishEyeGetRayDir(FishEyeClass *self, float x, float y);
static RayStruct	fishEyeGetRay(CameraClass *self, float x, float y);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static FishEyeClass       _descr = { /* FishEyeClass */
  { /* CameraClass */
    { /* Class */
      sizeof(FishEyeClass),
      0, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
      &fishEyeConstructor,
      &fishEyeDestructor,
    },
    0, /* setEye */
    0, /* setLookAt */
    0, /* setUp */
    &fishEyeGetRay, /* getRay */
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
  {
    180, /* mPsi */
    0 /* mRSquared */
  }
};

Class *FishEye = (Class*) &_descr;

static void     fishEyeConstructor(FishEyeClass *self,
                                             va_list *list)
{
  Camera->__constructor__(self, list);
}

static void     fishEyeDestructor(FishEyeClass *self)
{
  Camera->__destructor__(self);
}

void		*fishEyeUnserialize(SceneSerializerClass *serializer,
                                    xmlNodePtr node)
{
  FishEyeClass	*self;
  char		*strPsi;

  self = new(FishEye);
  if (cameraUnserializeSet(serializer, node, self) != 0)
    return (0);
  serializer->setStrsFromProp(serializer, node, "psi", &strPsi, NULL);
  if (setFloatFromStr(strPsi, &self->private.mPsi) != 0)
  {
    fprintf(stderr, "Error: bad attributes\n");
    return (0);
  }
  return (self);
}

/* vvv */

/*
 * dir = (sin_psi * cos_alpha * u) + (sin_psi * sin_alpha * v) - (cos_psi * w)
 */
static Vector3D	fishEyeGetRayDir(FishEyeClass *self, float x, float y)
{
  Point2D p = {2. / ((float)RTEngineGetSingletonPtr()->getSceneWidth()) * x,
              2. / ((float)RTEngineGetSingletonPtr()->getSceneHeight()) * y};
  self->private.mRSquared = p.x * p.x + p.y * p.y;
  if (self->private.mRSquared > 1.)
    return ((Vector3D){DeathNumber, 0, 0});
  float			r = sqrt(self->private.mRSquared);
  float			psi = r * self->private.mPsi * PiOn180;
  float			sin_psi = sinf(psi);
  float			cos_psi = cosf(psi);
  float			sin_alpha = p.y / r;
  float			cos_alpha = p.x / r;
  Vector3D	u = CAMERA(self)->protected.mU;
  Vector3D	v = CAMERA(self)->protected.mV;
  Vector3D	w = CAMERA(self)->protected.mW;
  Vector3D	dir;
  vectOpTimes(&u, sin_psi * cos_alpha);
  vectOpTimes(&v, sin_psi * sin_alpha);
  vectOpTimes(&w, cos_psi);
  dir = u;
  vectOpPlus(&dir, &v);
  vectOpMinus(&dir, &w);
  return (dir);
}

static RayStruct	fishEyeGetRay(CameraClass *self, float x, float y)
{
  RayStruct		ray;

  ray.origin = self->protected.mEye;
  ray.direction = fishEyeGetRayDir(FISHEYE(self), x, y);
  return (ray);
}

/* ^^^ */

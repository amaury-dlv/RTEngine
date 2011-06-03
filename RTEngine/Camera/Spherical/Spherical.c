#include "Spherical.h"
#include "RTEngine.h"
#include "SceneSerializer.h"
#include "Utils.h"
#include "Maths.h"
#include "Point2D.h"

static void     sphericalConstructor(SphericalClass* self, va_list* list);
static void     sphericalDestructor();
static Vector3D	sphericalGetRayDir(SphericalClass* self, float x, float y);
static RayStruct	sphericalGetRay(CameraClass* self, float x, float y);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static SphericalClass       _descr = { /* SphericalClass */
  { /* CameraClass */
    { /* Class */
      sizeof(SphericalClass),
      0, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
      &sphericalConstructor,
      &sphericalDestructor,
    },
    0, /* setEye */
    0, /* setLookAt */
    0, /* setUp */
    &sphericalGetRay, /* getRay */
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
    180 /* mLambda */
  }
};

Class* Spherical = (Class*) &_descr;

static void     sphericalConstructor(SphericalClass* self,
                                             va_list* list)
{
  Camera->__constructor__(self, list);
}

static void     sphericalDestructor(SphericalClass* self)
{
  Camera->__destructor__(self);
}

void		*sphericalUnserialize(SceneSerializerClass *serializer,
                                      xmlNodePtr node)
{
  SphericalClass	*self;
  char			*strPsi, *strLambda;

  self = new(Spherical);
  if (cameraUnserializeSet(serializer, node, self) != 0)
    return (0);
  serializer->setStrsFromProp(serializer, node,
      "psi", &strPsi, "lambda", &strLambda, NULL);
  if (setFloatFromStr(strPsi, &self->private.mPsi) != 0
      || setFloatFromStr(strLambda, &self->private.mLambda) != 0)
  {
    fprintf(stderr, "Error: bad attributes\n");
    return (0);
  }
  return (self);
}

/* vvv */

static Vector3D	sphericalGetRayDir(SphericalClass* self, float x, float y)
{
  Point2D p = {2. / ((float)RTEngineGetSingletonPtr()->getSceneWidth()) * x,
               2. / ((float)RTEngineGetSingletonPtr()->getSceneHeight()) * y};
  float			lambda = p.x * self->private.mLambda * PiOn180;
  float			psi = p.y * self->private.mPsi * PiOn180;
  float			phi = Pi - lambda;
  float			theta = .5 * Pi - psi;
  float			sin_phi = sinf(phi);
  float			cos_phi = cosf(phi);
  float			sin_theta = sinf(theta);
  float			cos_theta = cosf(theta);
  Vector3D	u = CAMERA(self)->protected.mU;
  Vector3D	v = CAMERA(self)->protected.mV;
  Vector3D	w = CAMERA(self)->protected.mW;
  vectOpTimes(&u, sin_theta * sin_phi);
  vectOpTimes(&v, cos_theta);
  vectOpTimes(&w, sin_theta * cos_phi);
  Vector3D	dir = u;
  vectOpPlus(&dir, &v);
  vectOpMinus(&dir, &w);
  dir.x = -dir.x;
  dir.y = -dir.y;
  dir.z = -dir.z;
  return (dir);
}

static RayStruct	sphericalGetRay(CameraClass* self, float x, float y)
{
  RayStruct		ray;

  ray.origin = self->protected.mEye;
  ray.direction = sphericalGetRayDir(SPHERICAL(self), x, y);
  return (ray);
}

/* ^^^ */

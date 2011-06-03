#include "Camera.h"
#include "Pinhole.h"
#include "SceneSerializer.h"

static void     CameraConstructor(CameraClass* self, va_list* list);
static void     CameraDestructor(CameraClass* self);
static void	cameraSetEye(CameraClass* self, double x, double y, double z);
static void	cameraSetLookAt(CameraClass* self,
                                double x, double y, double z);
static void	cameraSetUp(CameraClass* self, double x, double y, double z);
static bool	cameraHandleSpecialCases(CameraClass* self);
static void	cameraComputeUVW(CameraClass* self);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static CameraClass       _descr = { /* CameraClass */
  { /* Class */
    sizeof(CameraClass),
    0, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
    &CameraConstructor,
    &CameraDestructor,
  },
  0, /* setEye */
  0, /* setLookAt */
  0, /* setUp */
  0, /* getRay */
  0, /* computeUVW */
  {
    {0, 0, 0}, /* mEye */
    {0, 0, 0}, /* mLookAt */
    {0, 0, 0}, /* mUp */
    {0, 0, 0}, /* mU */
    {0, 0, 0}, /* mV */
    {0, 0, 0} /* mW */
  }
};

Class* Camera = (Class*) &_descr;

static void     CameraConstructor(CameraClass* self,
                                             va_list* list)
{
  self->setEye = &cameraSetEye;
  self->setLookAt = &cameraSetLookAt;
  self->setUp = &cameraSetUp;
  self->computeUVW = &cameraComputeUVW;
}

static void     CameraDestructor(CameraClass* self)
{
}

int	cameraUnserializeSet(SceneSerializerClass *serializer,
                                     xmlNodePtr node,
                                     CameraClass *self)
{
  char		*strEye;
  char		*strLookAt;
  char		*strUp;

  serializer->setStrsFromProp(serializer, node, "eye", &strEye,
    "lookat", &strLookAt, "up", &strUp, NULL);
  if (vectSetFromStr(&(self->protected.mEye), strEye) != 0
   || vectSetFromStr(&(self->protected.mLookAt), strLookAt) != 0
   || vectSetFromStr(&(self->protected.mUp), strUp) != 0)
  {
    fprintf(stderr, "Error: bad attributes\n");
    return (-1);
  }
  cameraComputeUVW(self);
  return (0);
}

static void	cameraSetEye(CameraClass* self, double x, double y, double z)
{
  self->protected.mEye.x = x;
  self->protected.mEye.y = y;
  self->protected.mEye.z = z;
  self->computeUVW(self);
}

static void	cameraSetLookAt(CameraClass* self,
                                double x, double y, double z)
{
  self->protected.mLookAt.x = x;
  self->protected.mLookAt.y = y;
  self->protected.mLookAt.z = z;
  self->computeUVW(self);
}

static void	cameraSetUp(CameraClass* self, double x, double y, double z)
{
  self->protected.mUp.x = x;
  self->protected.mUp.y = y;
  self->protected.mUp.z = z;
  self->computeUVW(self);
}

/* vvv */

static bool		cameraHandleSpecialCases(CameraClass* self)
{
  if (self->protected.mEye.x == self->protected.mLookAt.x
      && self->protected.mEye.z == self->protected.mLookAt.z
      && self->protected.mEye.y > self->protected.mLookAt.y)
  {
    self->protected.mU = (Vector3D){0., 0., 1.};
    self->protected.mV = (Vector3D){1., 0., 0.};
    self->protected.mW = (Vector3D){0., 1., 0.};
    return (true);
  }
  else if (self->protected.mEye.x == self->protected.mLookAt.x
      && self->protected.mEye.z == self->protected.mLookAt.z
      && self->protected.mEye.y < self->protected.mLookAt.y)
  {
    self->protected.mU = (Vector3D){1., 0., 0.};
    self->protected.mV = (Vector3D){0., 0., 1.};
    self->protected.mW = (Vector3D){0., -1., 0.};
    return (true);
  }
  return (false);
}

/*
 * This function  has to be called each time eye, up or lookAt is modified
 *
 * w = (eye - lookat) / || eye - lookat ||
 * u = (up * w) / || up * w||
 * v = u * w
 * '*' meaning Cross Product
 */
static void		cameraComputeUVW(CameraClass* self)
{
  if (cameraHandleSpecialCases(self))
    return ;
  vectOpEqual(&(self->protected.mW), &(self->protected.mEye));
  vectOpMinus(&(self->protected.mW), &(self->protected.mLookAt));
  vectNormalize(&(self->protected.mW));
  vectOpEqual(&(self->protected.mU), &(self->protected.mUp));
  vectCrossProduct(&(self->protected.mU), &(self->protected.mW));
  vectNormalize(&(self->protected.mU));
  vectOpEqual(&(self->protected.mV), &(self->protected.mW));
  vectCrossProduct(&(self->protected.mV), &(self->protected.mU));
}

/* ^^^ */

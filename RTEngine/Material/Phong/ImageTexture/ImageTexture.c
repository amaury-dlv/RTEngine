#include <string.h>
#include <math.h>

#include "ImageTexture.h"
#include "Lambertian.h"
#include "GlossySpecular.h"
#include "Utils.h"
#include "TracerManager.h"
#include "Whitted.h"
#include "PerlinNoise.h"
#include "PathTracer.h"

static void	imageTextureConstructor(ImageTextureClass *self, va_list *list);
static void	imageTextureDestructor(ImageTextureClass *self);
static void	imageTextureSetCr(ImageTextureClass *self, float cr);
static void	imageTextureSetKr(ImageTextureClass *self, float kr);
static void	imageTextureSetObjType(ImageTextureClass *self, char *strObjType);
static void	imageTextureReadFile(ImageTextureClass *self,
				     const char* file_name);
static float	imageTextureShade(ImageTextureClass *self,
				  HitRecordStruct *hitRecord,
				  LightClass *currentLight);
static Color	imageTextureShadeRGB(ImageTextureClass *self,
				     HitRecordStruct *hitRecord);
static Color	imageTextureShadePath(ImageTextureClass *self,
				      HitRecordStruct *hitRecord);
static void	imageTextureGetTexelSphere(ImageTextureClass *self,
					   HitRecordStruct *hitRecord);
static void	imageTextureGetTexelRectangle(ImageTextureClass *self,
				       HitRecordStruct *hitRecord);
static void	imageTextureGetTexelCylinder(ImageTextureClass *self,
				      HitRecordStruct *hitRecord);
static bool	imageTextureGetTextureColor(ImageTextureClass *self, Color *color);
static void	imageTexturePPMHeader(FILE *file);
static void	imageTextureGetImageRes(ImageTextureClass *self, FILE *file);
static void	imageTextureAllocateImageBuffer(ImageTextureClass *self,
                                                FILE *file);
static void	imageTextureGetImageData(ImageTextureClass *self, FILE *file);
static void	imageTextureScale(Vector3D* hitPoint, float *mScale);

static ImageTextureClass       _descr = { /* ImageTextureClass */
  { /* PhongClass */
    { /* MaterialClass */
      { /* Class */
        sizeof(ImageTextureClass),
        __RTTI_IMGTEXTURE, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
        &imageTextureConstructor,
        &imageTextureDestructor,
      },
      /* public */
      &imageTextureShade, /* phongShade */
      &imageTextureShadeRGB,
      0, /* shadeAreaLight */
      0, /* shadePath */
      /* public */
      {0, 0, 0}, /* color */
      /* protected */
      {
        0, /* normalBuffer */
        0, /* pointBuffer */
        0 /* rayBuffer */
      }
    },
    0, /* setters */
    0, /* setters */
    0, /* setters */
    { /* private */
      0, /* mDiffuseBrdf */
      0 /* mGlossySpecularBrdf */
    }
  },
  &imageTextureSetKr,
  &imageTextureSetCr,
  &imageTextureSetObjType,
  &imageTextureReadFile,
  0, /* imageGetTexelCoordinates() = 0 */
  &imageTextureGetTextureColor,
  &imageTextureScale,
  { /* private */
    0, /* mPerfectSpecular */
    0, /* mImageBuffer */
    0, /* mScale */
    0, /* mBufferSize */
    0, /* mHres */
    0, /* mVres */
    0, /* mRow */
    0, /* mCol */
    0, /* mInvMaxValue */
  }
};

Class *ImageTexture = (Class*) &_descr;

static void	imageTextureConstructor(ImageTextureClass *self, va_list *list)
{
  printf("ImageTextureClassConstructor\n");
  Phong->__constructor__(self, list);
  self->private.mPerfectSpecular = new(PerfectSpecular, eRegular);
  MATERIAL(self)->shade = &imageTextureShade;
  MATERIAL(self)->shadeRGB = &imageTextureShadeRGB;
  MATERIAL(self)->shadePath = &imageTextureShadePath;
}

static void	imageTextureDestructor(ImageTextureClass *self)
{
  printf("ImageTextureClassDestructor\n");
  delete(self->private.mPerfectSpecular);
}

void	*imageTextureUnserialize(SceneSerializerClass *serializer,
				 xmlNodePtr node)
{
  ImageTextureClass	*self;
  MaterialIdAssoc	*assoc;
  char			*strKr, *strCr, *strPath, *strObjType, *strScale;
  float			kr, cr;

  self = new(ImageTexture);
  if ((assoc = phongUnserializeSet(serializer, node, PHONG(self))) == 0)
    return (0);
  serializer->setStrsFromProp(serializer, node, "kr", &strKr, "cr",
      &strCr, "file", &strPath, "obj_type", &strObjType, NULL);
  if (setFloatFromStr(strKr, &kr) != 0 || setFloatFromStr(strCr, &cr) != 0)
  {
    fprintf(stderr, "Error: bad attributes\n");
    return (0);
  }
  strScale = (char*)xmlGetProp(node, XMLSTR("scale"));
  if (strScale != 0)
    if (setFloatFromStr(strScale, &self->private.mScale) != 0)
      return (0);
  self->setKr(self, kr);
  self->setCr(self, cr);
  self->setObjType(self, strObjType);
  self->readFileImage(self, strPath);
  return (assoc);
}

static void     imageTexturePPMHeader(FILE *file)
{
  unsigned char ppm_type;

  if (fscanf(file, "P%c\n", &ppm_type) != 1)
  {
    printf("Invalid PPM signature\n");
    exit(1);
  }
  if (ppm_type != '6')
  {
    printf("Only binary PPM supported imageReadFile\n");
    exit(1);
  }
}

static void	imageTextureGetImageRes(ImageTextureClass *self, FILE *file)
{
  unsigned char dummy;
  int		hres;
  int		vres;

  while (fscanf(file, "#%c", &dummy))
    while (fgetc(file) != '\n');
  if (fscanf(file, "%d %d\n", &hres, &vres) != 2)
    exit(1);
  if (hres <= 0)
    exit(1);
  else
    printf("image hres = %d\n", hres);
  if (vres <= 0)
    exit(1);
  else
    printf("image vres = %d\n", vres);
  self->private.mHres = hres;
  self->private.mVres = vres;
}

static void	imageTextureAllocateImageBuffer(ImageTextureClass *self,
						FILE *file)
{
  int		max_value;

  if (fscanf(file, "%d\n", &max_value) != 1)
    exit(1);
  self->private.mInvMaxValue = 1.0 / (float)max_value;
  self->private.mImageBuffer = xmalloc(sizeof(*self->private.mImageBuffer)
				       * self->private.mHres
				       * self->private.mVres);
  if (!self->private.mImageBuffer)
    exit(1);
  memset(self->private.mImageBuffer, 0, sizeof(*self->private.mImageBuffer)
				       * self->private.mHres
				       * self->private.mVres);
}

static void	imageTextureGetImageData(ImageTextureClass *self, FILE *file)
{
  Color		imgColor;
  unsigned char red;
  unsigned char green;
  unsigned char blue;
  int		x, y, i;

  i = y = 0;
  while (y < self->private.mVres)
  {
    x = 0;
    while (x < self->private.mHres)
    {
      if (fscanf(file, "%c%c%c", &red, &green, &blue) != 3)
	exit(1);
      imgColor.r = red   * self->private.mInvMaxValue;
      imgColor.g = green * self->private.mInvMaxValue;
      imgColor.b = blue  * self->private.mInvMaxValue;
      memcpy(&self->private.mImageBuffer[i], &imgColor,
             sizeof(*self->private.mImageBuffer));
      i++;
      x++;
    }
    y++;
  }
  self->private.mBufferSize = i - 1;
}

static void imageTextureReadFile(ImageTextureClass *self,
				 const char* file_name)
{
  FILE* file = fopen(file_name, "rb");

  if (!file)
  {
    printf("imageReadFile: could not open file\n");
    exit(1);
  }
  printf("imageReadFile: file opened\n");
  imageTexturePPMHeader(file);
  imageTextureGetImageRes(self, file);
  imageTextureAllocateImageBuffer(self, file);
  imageTextureGetImageData(self, file);
  fclose(file);
  printf("finished reading PPM file\n");
}

static void	imageTextureSetObjType(ImageTextureClass *self,
				       char *strObjType)
{
  printf("imageSetObjType\n");
  if (!strcmp("sphere", strObjType))
    self->getTexelCoordinates = &imageTextureGetTexelSphere;
  if (!strcmp("rectangle", strObjType))
    self->getTexelCoordinates = &imageTextureGetTexelRectangle;
  if (!strcmp("cylinder", strObjType))
    self->getTexelCoordinates = &imageTextureGetTexelCylinder;
}

static void	imageTextureSetCr(ImageTextureClass *self, float cr)
{
  PERFECTSPEC(self->private.mPerfectSpecular)\
    ->setCr(PERFECTSPEC(self->private.mPerfectSpecular), cr);
}

static void	imageTextureSetKr(ImageTextureClass *self, float kr)
{
  PERFECTSPEC(self->private.mPerfectSpecular)\
    ->setKr(PERFECTSPEC(self->private.mPerfectSpecular), kr);
}

static float		imageTextureShade(ImageTextureClass *self,
                                   HitRecordStruct *hitRecord,
                                   LightClass *currentLight)
{
  return (MATERIAL(Phong)->shade(self, hitRecord, currentLight));
}

static Color		imageTextureShadeRGB(ImageTextureClass *self,
                                           HitRecordStruct *hitRecord)
{
  Color			color = {0, 0, 0};
  Color			texColor = {0, 0, 0};

  color = MATERIAL(Phong)->shadeRGB(self, hitRecord);
  self->scale(&hitRecord->localHitPoint, &self->private.mScale);
  self->getTexelCoordinates(self, hitRecord);
  if (self->getTextureColor(self, &texColor) == true)
    colorMultColor(&color, &texColor);
  return (color);
}

static Color		imageTextureShadePath(ImageTextureClass *self,
				       HitRecordStruct *hitRecord)
{
  Vector3D		wi, wo = hitRecord->ray.direction;
  vectInv(&wo);
  float			pdf;
  Color			fr = BRDFT(PRIV(mPerfectSpecular))->\
                             sampleFPath(BRDFT(PRIV(mPerfectSpecular)),
					 hitRecord, &wo, &wi, &pdf);
  RayStruct		reflectedRay = {hitRecord->hitPoint, wi};

  colorMultColor(&fr, COLORPTR(PATHTRACER(TracerManagerGetTracer(ePathTracer))\
    ->trace(PATHTRACER(TracerManagerGetTracer(ePathTracer)),
	    &reflectedRay, hitRecord->reflectDepth + 1)));
  colorMult(&fr, vectDotProduct(&hitRecord->normal, &wi) / pdf);
  return (fr);
}

static void	imageTextureScale(Vector3D* hitPoint, float *mScale)
{
  hitPoint->x /= *mScale;
  hitPoint->y /= *mScale;
  hitPoint->z /= *mScale;
}

static bool	imageTextureGetTextureColor(ImageTextureClass *self,
					    Color *color)
{
  int		i;

  i = self->private.mRow + self->private.mHres
    * (self->private.mVres - self->private.mCol - 1);
  if (i < 0)
    i = -i;
  if (i < self->private.mBufferSize)
  {
    memcpy(color, &self->private.mImageBuffer[i], sizeof(Color));
    return (true);
  }
  return (false);
}

static void	imageTextureGetTexelRectangle(ImageTextureClass *self,
					      HitRecordStruct *hitRecord)
{
  float		u;
  float		v;

  u = (hitRecord->localHitPoint.z + 1) / 2;
  v = (hitRecord->localHitPoint.x + 1) / 2;
  self->private.mCol = (int)((self->private.mHres - 1) * u);
  self->private.mRow = (int)((self->private.mVres - 1) * v);
}

static void	imageTextureGetTexelSphere(ImageTextureClass *self,
					   HitRecordStruct *hitRecord)
{
  float		phi;
  float		theta;
  float		u;
  float		v;

  phi = acos(hitRecord->localHitPoint.y);
  theta = atan2(hitRecord->localHitPoint.x, hitRecord->localHitPoint.z);
  if (phi < 0.0)
    phi += TwoPi;
  u = phi * InvTwoPi;
  v = 1.0 - theta * InvPi;
  self->private.mCol = (int)((self->private.mHres - 1) * u);
  self->private.mRow = (int)((self->private.mVres - 1) * v);
}

static void	imageTextureGetTexelCylinder(ImageTextureClass *self,
					     HitRecordStruct *hitRecord)
{
  float		phi;
  float		u;
  float		v;

  phi = atan2(hitRecord->localHitPoint.x, hitRecord->localHitPoint.z);
  if (phi < 0.0)
    phi += TwoPi;
  u = phi / TwoPi;
  v = hitRecord->localHitPoint.y + 1.0 / 2.0;
  self->private.mCol = (int)((self->private.mHres - 1) * u);
  self->private.mRow = (int)((self->private.mVres - 1) * v);
}

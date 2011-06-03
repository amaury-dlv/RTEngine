#include <BmpGraphicsManager.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>

#include "RTTI.h"
#include "Color.h"
#include "RTEngine.h"

static void     BmpGraphicsManagerConstructor();
static void     BmpGraphicsManagerDestructor();
static void	bmpGraphicsManagerDisplay(ColorBufferClass* colorBuffer);
static void	bmpGraphicsManagerEnd(void);
static char	*bmpGetImageName(void);
static void	bmpFillImage(ColorBufferClass* ColorBuffer,
                             int fd, int width, int height);
static void	bmpPutHeader(int fd, int width, int height);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static BmpGraphicsManagerClass       _descr = { /* bmpGraphicsManagerClass */
  { /* Graphics Manager */
    { /* Class */
      sizeof(BmpGraphicsManagerClass),
      0, /* RTTI */
      &BmpGraphicsManagerConstructor,
      &BmpGraphicsManagerDestructor,
    },
    VOID(bmpGraphicsManagerDisplay),
    VOID(bmpGraphicsManagerEnd),
    {
      0, /* mWidth */
      0, /* mHeight */
      0 /* mColorBuffer */
    }
  },
  {
    0, /* colorBuffer */
    0, /* imgFileName */
    0 /* imgFd */
  }
};

Class* BmpGraphicsManager = (Class*) &_descr;

static void     BmpGraphicsManagerConstructor(BmpGraphicsManagerClass* self,
                                             va_list* list)
{
  GraphicsManager->__constructor__(self, list);
  self->private.imgFileName = bmpGetImageName();
  printf("%s\n", self->private.imgFileName);
  if ((self->private.imgFd =
        open(self->private.imgFileName,
          O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1)
    raise("Can't create output file.\n");
}

static void     		BmpGraphicsManagerDestructor()
{
  BmpGraphicsManagerClass*      this;

  this = VOID(GraphicsManagerGetSingletonPtr());
  free(this->private.imgFileName);
  if (close(this->private.imgFd) == -1)
    fprintf(stderr, "Warning: fail to close the screenshot fd.\n");
}

static void	bmpPutHeader(int fd, int width, int height)
{
  t_bitmap_fileheader	fileheader;
  t_bitmap_infoheader 	infoheader;

  infoheader.biSize = (unsigned int)(sizeof(infoheader));
  infoheader.biWidth = (unsigned int)width;
  infoheader.biHeight = (unsigned int)height;
  infoheader.biPlanes = (unsigned short)1;
  infoheader.biBitCount = (unsigned short)24;
  infoheader.biCompression = (unsigned int)0;
  infoheader.biSizeImage = (unsigned int)
    (width * height * infoheader.biBitCount / 8);
  infoheader.biXPelsPerMeter = (unsigned int)0;
  infoheader.biYPelsPerMeter = (unsigned int)0;
  infoheader.biClrUsed = (unsigned int)0;
  infoheader.biClrImportant = (unsigned int)0;
  fileheader.bfType = (unsigned short)0x4D42;
  fileheader.bfSize = (unsigned int)
    (infoheader.biSizeImage + sizeof(fileheader) + sizeof(infoheader));
  fileheader.bfReserved1 = (unsigned short)0;
  fileheader.bfReserved2 = (unsigned short)0;
  fileheader.bfOffBits = (unsigned int)
    (sizeof(fileheader) + sizeof(infoheader));
  if (write(fd, &fileheader, sizeof(fileheader)) == -1
      || write(fd, &infoheader, sizeof(infoheader)) == -1)
    raise("Can't write output file.\n");
}

static void		bmpFillImage(ColorBufferClass* ColorBuffer, int fd,
    				       int width, int height)
{
  int			x;
  int			y;
  unsigned char		tmp[3];
  unsigned int		tmpColor;

  bmpPutHeader(fd, width, height);
  y = 0;
  while (y < height)
  {
    x = 0;
    while (x < width)
    {
      tmpColor =
        colorGetUInt(ColorBuffer->getColorPtr(ColorBuffer,
            x + (height - 1 - y) * width));
      tmp[0] = (tmpColor >>  0) & 0xff;
      tmp[1] = (tmpColor >>  8) & 0xff;
      tmp[2] = (tmpColor >> 16) & 0xff;
      if (write(fd, tmp, 3) == -1)
        raise("Can't write output file.\n");
      ++x;
    }
    ++y;
  }
}

static char*	bmpGetImageName(void)
{
  time_t	current_timestamp;
  struct tm*	time_data;
  static char*	ret = NULL;

  if (ret == NULL)
  {
    if ((current_timestamp = time(NULL)) == -1)
      raise("Can't get current timestamp.\n");
    if ((time_data = localtime(&current_timestamp)) == NULL)
      raise("Can't get current time data.\n");
    if (asprintf(&ret, RT_TITLE "_%02d-%02d-%04d_%02d-%02d-%02d.bmp",
          time_data->tm_mday, time_data->tm_mon, time_data->tm_year + 1900,
          time_data->tm_hour, time_data->tm_min, time_data->tm_sec) == -1)
      raise("Can't gen output file name.\n");
  }
  return (ret);
}

static void	bmpGraphicsManagerDisplay(ColorBufferClass* colorBuffer)
{
  BmpGraphicsManagerClass*      this;

  this = VOID(GraphicsManagerGetSingletonPtr());
  this->private.colorBuffer = colorBuffer;
}

static void			bmpGraphicsManagerEnd(void)
{
  BmpGraphicsManagerClass*      this;

  this = VOID(GraphicsManagerGetSingletonPtr());
  bmpFillImage(this->private.colorBuffer, this->private.imgFd,
      GRAPHICSMGR(this)->protected.mWidth,
      GRAPHICSMGR(this)->protected.mHeight);
}

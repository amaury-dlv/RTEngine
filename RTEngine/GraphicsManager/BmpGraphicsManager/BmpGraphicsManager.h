#ifndef  BMPGRAPHICMANAGER_H_
# define BMPGRAPHICMANAGER_H_

# ifndef NO_GRAPHICS
#  define _GNU_SOURCE
#  include <stdio.h>
#  include "OOC.h"
#  include "GraphicsManager.h"
#  include "Color.h"

typedef struct
{
  unsigned short	bfType;
  unsigned int		bfSize;
  unsigned short	bfReserved1;
  unsigned short	bfReserved2;
  unsigned int		bfOffBits;
} __attribute__ ((__packed__))	t_bitmap_fileheader;

typedef struct
{
  unsigned int		biSize;
  unsigned int		biWidth;
  unsigned int		biHeight;
  unsigned short	biPlanes;
  unsigned short	biBitCount;
  unsigned int		biCompression;
  unsigned int		biSizeImage;
  unsigned int		biXPelsPerMeter;
  unsigned int		biYPelsPerMeter;
  unsigned int		biClrUsed;
  unsigned int		biClrImportant;
} __attribute__ ((__packed__))	t_bitmap_infoheader;

typedef struct BmpGraphicsManagerClass	BmpGraphicsManagerClass;
struct                          	BmpGraphicsManagerClass
{
  /* SuperClass : */
  GraphicsManagerClass			__base__;
  /* public : */
  /* private : */
  struct
  {
    ColorBufferClass*			colorBuffer;
    char*				imgFileName;
    int					imgFd;
  } private;
};

extern Class*				BmpGraphicsManager;

# endif

#endif /* !BMPGRAPHICMANAGER_H_ */

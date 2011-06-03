# include <mlx.h>
# include <X11/keysym.h>

# include "RTTI.h"
# include "Color.h"
# include "RTEngine.h"
# include "MlxGraphicsManager.h"

static void     MlxGraphicsManagerConstructor();
static void     MlxGraphicsManagerDestructor();
static void	mlxColorBufferToMlxImg(ColorBufferClass* colorBuffer);
static void	mlxGraphicsManagerDisplay(ImageBufferClass* colorBuffer);
static void	mlxGraphicsManagerEnd(void);
static int	mlxHandleExpose(MlxGraphicsManagerClass *self);
static int	mlxHandleKey(int keyCode, MlxGraphicsManagerClass *self);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static MlxGraphicsManagerClass       _descr = { /* MlxGraphicsManagerClass */
  { /* Graphics Manager */
    { /* Class */
      sizeof(MlxGraphicsManagerClass),
      0, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
      &MlxGraphicsManagerConstructor,
      &MlxGraphicsManagerDestructor,
    },
    &mlxGraphicsManagerDisplay,
    &mlxGraphicsManagerEnd,
    {
      0, /* mWidth */
      0, /* mHeight */
      0 /* mColorBuffer */
    }
  },
  {
    0, /* mMlxPtr */
    0, /* mWinPtr */
    0, /* mImgPtr */
    0 /* mImgDataPtr */
  }
};

Class* MlxGraphicsManager = (Class*) &_descr;

static void     MlxGraphicsManagerConstructor(MlxGraphicsManagerClass* self,
                                             va_list* list)
{
  GraphicsManager->__constructor__(self, list);
  if ((self->private.mMlxPtr = mlx_init()) == NULL)
    raise("Cannot initialize mlx\n");
  self->private.mWinPtr = mlx_new_window(self->private.mMlxPtr,
                                         GRAPHICSMGR(self)->protected.mWidth,
                                         GRAPHICSMGR(self)->protected.mHeight,
                                         RT_TITLE);
  self->private.mImgPtr = mlx_new_image(self->private.mMlxPtr,
                                        GRAPHICSMGR(self)->protected.mWidth,
                                        GRAPHICSMGR(self)->protected.mHeight);
  self->private.mImgDataPtr =
    (unsigned *)mlx_get_data_addr(self->private.mImgPtr,
                                  &(int){0},
                                  &(int){0},
                                  &(int){0});
  mlx_key_hook(self->private.mWinPtr, &mlxHandleKey, self);
  mlx_expose_hook(self->private.mWinPtr, &mlxHandleExpose, self);
  GRAPHICSMGR(self)->protected.mColorBuffer =
    new(ColorBuffer,
        GRAPHICSMGR(self)->protected.mWidth,
        GRAPHICSMGR(self)->protected.mHeight);
}

static void     MlxGraphicsManagerDestructor()
{
}

static int	mlxHandleExpose(MlxGraphicsManagerClass *self)
{
  mlx_put_image_to_window(self->private.mMlxPtr,
                          self->private.mWinPtr,
                          self->private.mImgPtr,
                          0, 0);
  return (EXIT_SUCCESS);
}

static int	mlxHandleKey(int keyCode, MlxGraphicsManagerClass *self)
{
  if (keyCode == XK_Escape)
    exit(EXIT_SUCCESS);
  return (EXIT_SUCCESS);
}

static void	mlxColorBufferToMlxImg(ColorBufferClass* colorBuffer)
{
  MlxGraphicsManagerClass*	this;
  int				i;
  Color				color;

  i = 0;
  this = VOID(GraphicsManagerGetSingletonPtr());
  while (i < GRAPHICSMGR(this)->protected.mWidth
      * GRAPHICSMGR(this)->protected.mHeight)
  {
    color = colorBuffer->getColor(colorBuffer, i);
    this->private.mImgDataPtr[i] = colorGetUInt(&color);
    ++i;
  }
}

/*
 * The imageBuffer is converted only if it's not already a colorBuffer
 */

static void	mlxGraphicsManagerDisplay(ImageBufferClass* imageBuffer)
{
  MlxGraphicsManagerClass*	this;

  this = VOID(GraphicsManagerGetSingletonPtr());
  if (CLASS(imageBuffer)->__RTTI__ == __RTTI_COLORBUFFER)
    mlxColorBufferToMlxImg(COLORBUF(imageBuffer));
  else
  {
    imageBuffer->convertToColorBuffer(imageBuffer,
        GRAPHICSMGR(this)->protected.mColorBuffer);
    mlxColorBufferToMlxImg(GRAPHICSMGR(this)->protected.mColorBuffer);
  }
  mlx_put_image_to_window(this->private.mMlxPtr,
                          this->private.mWinPtr,
                          this->private.mImgPtr,
                          0, 0);
}

static void	mlxGraphicsManagerEnd(void)
{
  MlxGraphicsManagerClass*      this;

  this = VOID(GraphicsManagerGetSingletonPtr());
  mlx_loop(this->private.mMlxPtr);
}

#ifndef __IMAGETEXTURE_H__
# define __IMAGETEXTURE_H__

# include "Phong.h"
# include "PerfectSpecular.h"
# include "SceneSerializer.h"

# define IMAGETEXTURE(x)		((ImageTextureClass*) (x))

typedef struct ImageTextureClass	ImageTextureClass;
struct					ImageTextureClass
{
  /* SuperClass : */
  PhongClass			__base__;
  /* public : */
  /* public : */
  void				(*setKr)(ImageTextureClass *self, float kr);
  void				(*setCr)(ImageTextureClass *self, float cr);
  void				(*setObjType)(ImageTextureClass *self,
					      char *ObjType);
  void				(*readFileImage)(ImageTextureClass *self,
						 const char* file_name);
  void			(*getTexelCoordinates)(ImageTextureClass *self,
					       HitRecordStruct *hitRecord);
  bool				(*getTextureColor)(ImageTextureClass *self,
						   Color *color);
  void				(*scale)(Vector3D *hitPoint, float *mScale);
  /* private : */
  struct
  {
    PerfectSpecularClass	*mPerfectSpecular;
    Color			*mImageBuffer;
    float			mScale;
    int				mBufferSize;
    int				mHres;
    int				mVres;
    int				mRow;
    int				mCol;
    float			mInvMaxValue;
  } private;
};

extern Class*			ImageTexture;

void	*imageTextureUnserialize(SceneSerializerClass *serializer,
				 xmlNodePtr node);

#endif /* !__IMAGETEXTURE_H__ */

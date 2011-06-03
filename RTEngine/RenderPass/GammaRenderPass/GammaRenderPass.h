#ifndef  GAMMARENDERPASS_H_
# define GAMMARENDERPASS_H_

# include "OOC.h"
# include "RenderPass.h"
# include "Raycast.h"
# include "HitRecord.h"
# include "ColorBuffer.h"
# include "DepthBuffer.h"
# include "PointBuffer.h"
# include "NormalBuffer.h"
# include "MaterialBuffer.h"
# include "RayBuffer.h"
# include "BoolBuffer.h"

# define GAMMARPASS(x)	((GammaRenderPassClass*) (x))

# define DEFAULT_GAMMA	(1)

typedef struct GammaRenderPassClass	GammaRenderPassClass;
struct                          	GammaRenderPassClass
{
  /* SuperClass : */
  RenderPassClass		__base__;
  /* public : */
  /* private : */
  struct
  {
    ColorBufferClass		*mColorBuffer;
    float			mE; /* Gamma coeff */
  } private;
};

extern Class*			GammaRenderPass;

void	  	*gammaRenderPassUnserialize();

#endif /* !GAMMARENDERPASS_H_ */

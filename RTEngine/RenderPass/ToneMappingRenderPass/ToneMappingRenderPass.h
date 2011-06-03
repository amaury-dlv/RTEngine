#ifndef  TONEMAPPINGRENDERPASS_H_
# define TONEMAPPINGRENDERPASS_H_

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

# define TONEMAPPINGRPASS(x)	((ToneRenderPassClass*) (x))

# define DEFAULT_TONE_MAPPING	(1)

typedef struct ToneMappingRenderPassClass	ToneMappingRenderPassClass;
struct                          		ToneMappingRenderPassClass
{
  /* SuperClass : */
  RenderPassClass		__base__;
  /* public : */
  /* private : */
  struct
  {
    ColorBufferClass		*mColorBuffer;
    float			mCoeff; /* ToneMapping coeff */
  } private;
};

extern Class*			ToneMappingRenderPass;

void	  	*toneMappingRenderPassUnserialize();

#endif /* !TONEMAPPINGRENDERPASS_H_ */

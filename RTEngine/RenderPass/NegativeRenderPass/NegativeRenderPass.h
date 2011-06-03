#ifndef  NEGATIVERENDERPASS_H_
# define NEGATIVERENDERPASS_H_

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

# define NEGATIVERPASS(x)	((NegativeRenderPassClass*) (x))

typedef struct NegativeRenderPassClass		NegativeRenderPassClass;
struct                          		NegativeRenderPassClass
{
  /* SuperClass : */
  RenderPassClass		__base__;
  /* public : */
  /* private : */
  struct
  {
    ColorBufferClass		*mColorBuffer;
  } private;
};

extern Class*			NegativeRenderPass;

void	  	*negativeRenderPassUnserialize();

#endif /* !NEGATIVERENDERPASS_H_ */

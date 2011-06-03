#ifndef __INSTRADIOSITYRENDERPASS_H__
# define __INSTRADIOSITYRENDERPASS_H__

# include "OOC.h"
# include "RenderPass.h"
# include "ColorBuffer.h"
# include "Sampler.h"
# include "Scene.h"
# include "HitRecord.h"
# include "Vector.h"
# include "MaterialBuffer.h"

# define INSTRADRPASS(x)	((InstRadiosityClass*) (x))

# define GETVPLFROMHIT(x)	(POINTLIGHT((x)->material))

typedef struct InstRadiosityClass	InstRadiosityClass;
struct                          	InstRadiosityClass
{
  /* SuperClass : */
  RenderPassClass		__base__;
  /* public : */
  /* private : */
  struct
  {
    ColorBufferClass		*mVPLBuffer;
    SceneClass			*mScene;
    VectorClass			*mLightVect;
    SamplerClass		*mFirstBounceSampler;
    VectorClass			*mVPLVect;
    VectorClass			*mHitRecVect;
    ColorBufferClass		*mCurrentState;
    MaterialBufferClass		*mMaterialBuffer;
    ColorBufferClass		*mStepBuffer;
    ColorBufferClass		*mDisplayedBuffer;
    VectorClass			*mSpheresVec;
    bool			mHasBounce;
    float			mThreshold;
    float			mMapPower;
    float			mMapFactor;
  } private;
};

extern Class*			InstRadiosity;

void	*instRadiosityRenderPassUnserialize();

#endif /* !__INSTRADIOSITYRENDERPASS_H__ */

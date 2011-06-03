#ifndef __RENDERINGSCENE_H__
# define __RENDERINGSCENE_H__

# include "OOC.h"
# include "RTTI.h"
# include "Vector.h"
# include "Scene.h"
# include "RenderPass.h"
# include "ImageBuffer.h"
# include "ColorBuffer.h"
# include "RenderPass.h"
# include "GBuffer.h"

typedef struct RenderingSceneClass	RenderingSceneClass;
struct                          	RenderingSceneClass
{
  /* SuperClass : */
  Class				__base__;
  /* public : */
  void				(*addRenderPass)(RenderPassClass* renderPass);
  RenderPassClass		*(*getRenderPass)(int renderPassType);
  void				(*render)(SceneClass* scene);
  GBufferClass*			(*getGBuffer)(void);
  SceneClass*			(*getCurrentScene)(void);
  ColorBufferClass*		(*getCurrentState)(void);
  /* private : */
  struct
  {
    ColorBufferClass*			mCurrentState;
    GBufferClass*			mGBuffer;
    SceneClass*				mScene;
  } private;
};

typedef struct RenderPassTab		RenderPassTab;
struct					RenderPassTab
{
  char					*name;
  void					*renderPassPtr;
  RTTIType				rtti;
  bool					activated;
};

RenderingSceneClass*		RenderingSceneGetSingletonPtr(void);

extern Class*			RenderingScene;
extern RenderPassTab		RenderPasses[];
extern char			*charColors[];

#endif /* !__RENDERINGSCENE_H__ */

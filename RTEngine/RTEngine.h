#ifndef RTENGINE_H_
# define RTENGINE_H_

# include "OOC.h"
# include "Dequeue.h"
# include "Vector.h"
# include "RenderPass.h"
# include "SamplerManager.h"
# include "TracerManager.h"
# include "GraphicsManager.h"
# include "RenderingScene.h"
# include "Scene.h"

typedef struct RTEngineClass	RTEngineClass;
struct                          RTEngineClass
{
  /* SuperClass : */
  Class				__base__;
  /* public : */
  void				(*init)(void);
  void				(*addScene)(SceneClass* scene);
  SceneClass  			*(*getScene)(void);
  void				(*renderScene)();
  void				(*renderAllScenes)();
  void				(*addRenderPass)(RenderPassClass* renderPass);
  void				(*setSceneWidth)(int width);
  void				(*setSceneHeight)(int height);
  int				(*getSceneWidth)(void);
  int				(*getSceneHeight)(void);
  void				(*setSceneSize)(char *size);
  void				(*setSceneDepth)(int depth);
  int				(*getSceneDepth)(void);
  void				(*setLightAttenuation)(char *str);
  int				(*getLightAttenuation)(void);
  void				(*setSamplerShared)(char *str);
  int				(*getSamplerShared)(void);
  void				(*setDefaultSampler)(char *str);
  int				(*getDefaultNumSamples)(void);
  int				(*getDefaultNumSets)(void);
  /* private : */
  struct
  {
    SamplerManagerClass*	mSamplerManager;
    TracerManagerClass*		mTracerManager;
    RenderingSceneClass*	mCurrentScene;
    GraphicsManagerClass*	mGraphicsManager;
    DequeueClass*		mScenes;
    int				mSceneWidth;
    int				mSceneHeight;
    int				mDepth;
    int				mLightAttenuation;
    int				mSamplerShared;
    int				mDefaultNumSamples;
    int				mDefaultNumSets;
  } private;
};

RTEngineClass*			RTEngineGetSingletonPtr(void);

extern Class*			RTEngine;

#endif /* ! RTENGINE_H_ */

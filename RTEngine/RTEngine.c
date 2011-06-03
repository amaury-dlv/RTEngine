#include <stdio.h>

#include "RTEngine.h"
#include "MlxGraphicsManager.h"
#include "BmpGraphicsManager.h"
#include "SdlGraphicsManager.h"
#include "DummyGraphicsManager.h"
#include "OptionsManager.h"
#include "Utils.h"

static void     RTEngineConstructor(RTEngineClass* self, va_list* list);
static void     RTEngineDestructor();
static void	RTEngineInit(void);
static void	RTEngineAddScene(SceneClass* scene);
static SceneClass	*RTEngineGetScene(void);
static void	RTEngineRenderScene(void);
static void	RTEngineRenderAllScenes(void);
static void	RTEngineAddRenderPass(RenderPassClass* renderPass);
static int	RTEngineGetSceneWidth(void);
static int	RTEngineGetSceneHeight(void);
static void	RTEngineSetSceneWidth(int width);
static void	RTEngineSetSceneHeight(int height);
static void	RTEngineSetSceneSize(char *size);
static void	RTEngineSetSceneDepth(int depth);
static int	RTEngineGetSceneDepth(void);
static void	RTEngineSetLightAttenuation(char *str);
static int	RTEngineGetLightAttenuation(void);
static void	RTEngineSetSamplerShared(char *str);
static int	RTEngineGetSamplerShared(void);
static void	RTEngineSetDefaultSampler(char *str);
static int	RTEngineGetDefaultNumSamples(void);
static int	RTEngineGetDefaultNumSets(void);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static RTEngineClass       _descr = { /* RTEngineClass */
  { /* Class */
    sizeof(RTEngineClass),
    0, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
    &RTEngineConstructor,
    &RTEngineDestructor,
  },
  &RTEngineInit,
  &RTEngineAddScene,
  &RTEngineGetScene,
  &RTEngineRenderScene, /* renderScene */
  &RTEngineRenderAllScenes,
  &RTEngineAddRenderPass,
  &RTEngineSetSceneWidth,
  &RTEngineSetSceneHeight,
  &RTEngineGetSceneWidth,
  &RTEngineGetSceneHeight,
  &RTEngineSetSceneSize,
  &RTEngineSetSceneDepth,
  &RTEngineGetSceneDepth,
  &RTEngineSetLightAttenuation,
  &RTEngineGetLightAttenuation,
  &RTEngineSetSamplerShared,
  &RTEngineGetSamplerShared,
  &RTEngineSetDefaultSampler,
  &RTEngineGetDefaultNumSamples,
  &RTEngineGetDefaultNumSets,
  {
    0, /* mSamplerManager */
    0, /* mTracerManager */
    0, /* mCurrentScene */
    0, /* mGraphicsManager */
    0, /* mScenes */
    1280, 720,
    5, /* mDepth */
    0, /* mLightAttenuation */
    1, /* mSamplerShared */
    100, /* mDefaultNumSamples */
    83 /* mDefaultNumSets */
  }
};

Class* RTEngine = (Class*) &_descr;

static RTEngineClass* sSingleton = 0;

static void     RTEngineConstructor(RTEngineClass* self,
                                             va_list* list)
{
  if (sSingleton != 0)
    raise("Cannot new a singleton twice : RTEngine\n");
  sSingleton = self;
  self->private.mSamplerManager = new(SamplerManager);
  self->private.mScenes = new(Dequeue);
}

static void     RTEngineDestructor()
{
}

RTEngineClass*		RTEngineGetSingletonPtr(void)
{
  if (sSingleton == 0)
    raise("RTEngine not yet instanciated.\n");
  return (sSingleton);
}

static void		RTEngineInit(void)
{
  RTEngineClass		*this;
  char			*graphicsManagerStr;

  this = RTEngineGetSingletonPtr();
  this->setSceneSize(OptionsManagerGetSingletonPtr()->getOptionValue("Size"));
  this->private.mTracerManager = new(TracerManager);
  this->private.mCurrentScene = new(RenderingScene);
  graphicsManagerStr = OptionsManagerGetSingletonPtr()\
                       ->getOptionValue("Display");
#ifndef NO_GRAPHICS

  if (graphicsManagerStr == NULL
      || strcasecmp(graphicsManagerStr, "bmp") == 0)
    this->private.mGraphicsManager = new(BmpGraphicsManager);

#ifdef HAS_MLX
  else if (graphicsManagerStr == 0 || strcasecmp(graphicsManagerStr, "mlx") == 0)
    this->private.mGraphicsManager = new(MlxGraphicsManager);
#endif

#ifdef HAS_SDL
  else if (strcasecmp(graphicsManagerStr, "sdl") == 0)
    this->private.mGraphicsManager = new(SdlGraphicsManager);
#endif

  else if (strcasecmp(graphicsManagerStr, "none") == 0)
    this->private.mGraphicsManager = new(DummyGraphicsManager);

  else
    raise("No such GraphicsManager (%s).\n", graphicsManagerStr);
#else
    this->private.mGraphicsManager = new(DummyGraphicsManager);
#endif
}

static void		RTEngineAddScene(SceneClass* scene)
{
  RTEngineGetSingletonPtr()->private.mScenes\
    ->push_back(RTEngineGetSingletonPtr()->private.mScenes, scene);
}

static SceneClass	*RTEngineGetScene(void)
{
  return (RTEngineGetSingletonPtr()->private.mScenes\
    ->front(RTEngineGetSingletonPtr()->private.mScenes));
}

static void		RTEngineRenderScene(void)
{
  RTEngineClass*	this;

  this = RTEngineGetSingletonPtr();
  this->private.mCurrentScene->render(this->private.mScenes\
                                      ->front(this->private.mScenes));
  delete(this->private.mScenes->pop_front(this->private.mScenes));
}

static void		RTEngineRenderAllScenes(void)
{
  RTEngineClass*	this;

  this = RTEngineGetSingletonPtr();
  while (this->private.mScenes->front(this->private.mScenes))
  {
    this->private.mCurrentScene->render(this->private.mScenes\
      ->front(this->private.mScenes));
    delete(this->private.mScenes->pop_front(this->private.mScenes));
  }
}

static void		RTEngineAddRenderPass(RenderPassClass* renderPass)
{
  RTEngineClass*	this;

  this = RTEngineGetSingletonPtr();
  this->private.mCurrentScene->addRenderPass(renderPass);
}

static int		RTEngineGetSceneWidth(void)
{
  return (RTEngineGetSingletonPtr()->private.mSceneWidth);
}

static int		RTEngineGetSceneHeight(void)
{
  return (RTEngineGetSingletonPtr()->private.mSceneHeight);
}

static void		RTEngineSetSceneWidth(int width)
{
  RTEngineGetSingletonPtr()->private.mSceneWidth = width;
}

static void		RTEngineSetSceneHeight(int height)
{
  RTEngineGetSingletonPtr()->private.mSceneHeight = height;
}

static void		RTEngineSetSceneSize(char *size)
{
  RTEngineClass		*this;
  char			**w;

  if (size == 0)
    return ;
  this = RTEngineGetSingletonPtr();
  w = toWordtab(size, " ,x");
  if (wordtabSize(w) != 2)
    exit(1); // he oui
  this->private.mSceneWidth = atoi(w[0]);
  this->private.mSceneHeight = atoi(w[1]);
  if (this->private.mSceneHeight <= 0
      || this->private.mSceneWidth <= 0)
    exit(1);
}

static void		RTEngineSetSceneDepth(int depth)
{
  RTEngineClass		*this;

  this = RTEngineGetSingletonPtr();
  this->private.mDepth = depth;
}

static int		RTEngineGetSceneDepth(void)
{
  return (RTEngineGetSingletonPtr()->private.mDepth);
}

static void		RTEngineSetLightAttenuation(char *str)
{
  RTEngineClass		*this;

  this = RTEngineGetSingletonPtr();
  this->private.mLightAttenuation = !!atoi(str);
}

static int		RTEngineGetLightAttenuation(void)
{
  RTEngineClass		*this;

  this = RTEngineGetSingletonPtr();
  return (this->private.mLightAttenuation);
}

static void		RTEngineSetSamplerShared(char *str)
{
  RTEngineClass		*this;

  this = RTEngineGetSingletonPtr();
  RTEngineGetSingletonPtr()->private.mSamplerShared = !!atoi(str);
}

static int		RTEngineGetSamplerShared(void)
{
  return (RTEngineGetSingletonPtr()->private.mSamplerShared);
}

static void		RTEngineSetDefaultSampler(char *str)
{
  RTEngineClass		*this;
  char			*tmp;

  this = RTEngineGetSingletonPtr();
  tmp = strsep(&str, " x");
  this->private.mDefaultNumSamples = atoi(tmp);
  if (str)
    this->private.mDefaultNumSets = atoi(str);
}

static int		RTEngineGetDefaultNumSamples(void)
{
  return (RTEngineGetSingletonPtr()->private.mDefaultNumSamples);
}

static int		RTEngineGetDefaultNumSets(void)
{
  return (RTEngineGetSingletonPtr()->private.mDefaultNumSets);
}

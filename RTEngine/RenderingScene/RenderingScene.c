#include "RTTI.h"
#include "RTEngine.h"
#include "RenderingScene.h"
#include "FirstRenderPass.h"
#include "LightRenderPass.h"
#include "TransparencyRenderPass.h"
#include "ReflectionRenderPass.h"
#include "TextureRenderPass.h"
#include "BumpRenderPass.h"
#include "ShadeRGBRenderPass.h"
#include "AreaLightRenderPass.h"
#include "ColorBuffer.h"
#include "GraphicsManager.h"
#include "Maths.h"

static void     renderingSceneConstructor();
static void     renderingSceneDestructor(RenderingSceneClass *self);
static void	renderingSceneAddRenderPass(RenderPassClass *renderPass);
static void	renderingSceneRender(SceneClass *scene);
static RenderPassClass	*renderingSceneGetRenderPass(int renderPassRtti);
static GBufferClass*	renderingSceneGetGBuffer(void);
static SceneClass*	renderingSceneGetCurrentScene(void);
static ColorBufferClass*	renderingSceneGetCurrentState(void);

char	*charColors[] =
{
  "[0;095;040m",
  "[0;031;040m",
  "[0;092;040m",
  "[0;091;040m",
  "[0;032;040m",
  "[0;033;040m",
  "[0;093;040m",
  "[0;034;040m",
  "[0;094;040m",
  "[0;035;040m",
  "[0;036;040m",
  "[0;096;040m",
  "[0;037;040m",
  "[0;037;040m",
  "[0;037;040m",
  "[0;037;040m",
  "[0;037;040m",
  "[0;037;040m"
};

RenderPassTab	RenderPasses[] =
{
  {"Oversampling", 0, __RTTI_OVERSAMPLINGRENDERPASS, false},
  {"First", 0, __RTTI_FIRSTRENDERPASS, true},
  {"Bump", 0, __RTTI_BUMPRENDERPASS, false},
  {"Light", 0, __RTTI_LIGHTRENDERPASS, false},
  {"Shadow", 0, __RTTI_SHADOWRENDERPASS, false},
  {"Ambient occlusion", 0, __RTTI_AORENDERPASS, false},
  {"Texture", 0, __RTTI_TEXTURERENDERPASS, false},
  {"ShadeAll", 0, __RTTI_SHADERGBRENDERPASS, false},
  {"Transparency", 0, __RTTI_TRANSPARENCYRENDERPASS, false},
  {"Reflection", 0, __RTTI_REFLECTIONRENDERPASS, false},
  {"Area light", 0, __RTTI_AREALIGHTRENDERPASS, false},
  {"Instant radiosity", 0, __RTTI_INSTRADIOSITYRENDERPASS, false},
  {"Path tracing", 0, __RTTI_PATHTRACINGRENDERPASS, false},
  {"SSAO", 0, __RTTI_SSAORENDERPASS, false},
  {"Gamma", 0, __RTTI_GAMMARENDERPASS, false},
  {"Tone mapping", 0, __RTTI_TONEMAPPINGRENDERPASS, false},
  {"Negative", 0, __RTTI_NEGATIVERENDERPASS, false},
  {0, 0, 0, false}
};

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static RenderingSceneClass       _descr = { /* RenderingSceneClass */
  { /* Class */
    sizeof(RenderingSceneClass),
    0, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
    &renderingSceneConstructor,
    &renderingSceneDestructor,
  },
  &renderingSceneAddRenderPass,
  &renderingSceneGetRenderPass,
  &renderingSceneRender,
  &renderingSceneGetGBuffer,
  &renderingSceneGetCurrentScene,
  &renderingSceneGetCurrentState,
  {
    0,
    0,
    0,
  }
};

Class *RenderingScene = (Class*) &_descr;

static RenderingSceneClass *sSingleton = 0;

static void     renderingSceneConstructor(RenderingSceneClass *self,
    va_list *list)
{
  if (sSingleton != 0)
    raise("Cannot new a singleton twice : RenderingScene\n");
  sSingleton = self;
  self->private.mGBuffer = new(GBuffer);
  self->private.mCurrentState =
    new(ColorBuffer,
        RTEngineGetSingletonPtr()->getSceneWidth(),
        RTEngineGetSingletonPtr()->getSceneHeight());
  RenderPasses[1].renderPassPtr = new(FirstRenderPass);
  RenderPasses[6].renderPassPtr = new(ShadeRGBRenderPass);
}

static void	renderingSceneDestructor(RenderingSceneClass *self)
{
  int		i;

  i = 0;
  while (RenderPasses[i].name)
    delete(RenderPasses[i++].renderPassPtr);
  delete(self->private.mGBuffer);
}

RenderingSceneClass*		RenderingSceneGetSingletonPtr(void)
{
  if (sSingleton == 0)
    raise("RenderingScene not yet instanciated.\n");
  return (sSingleton);
}

static RenderPassClass		*renderingSceneGetRenderPass(int renderPassType)
{
  RenderingSceneClass*		this;
  int				i;

  i = 0;
  this = RenderingSceneGetSingletonPtr();
  while (RenderPasses[i].name)
  {
    if ((int) RenderPasses[i].rtti == renderPassType)
    {
      if (!RenderPasses[i].renderPassPtr)
        raise("Undefined render pass (%s)\n", RenderPasses[i].name);
      return (RenderPasses[i].renderPassPtr);
    }
    i++;
  }
  raise("Unknown render pass of type: %d\n", renderPassType);
  return (NULL);
}


static void			renderingSceneAddRenderPass(RenderPassClass *renderPass)
{
  RenderingSceneClass*		this;
  int				i;

  i = 0;
  this = RenderingSceneGetSingletonPtr();
  if (RenderPasses[0].activated)
    raise("OversamplingRenderPass is exclusive.\n");
  while (RenderPasses[i].name)
  {
    if ((int) RenderPasses[i].rtti == RTTI(renderPass))
    {
      if (RenderPasses[i].activated)
        raise("A render pass cannot be actived twice.\n");
      RenderPasses[i].renderPassPtr = renderPass;
      RenderPasses[i].activated = true;
      return ;
    }
    i++;
  }
  raise("Unknown render pass of type: %d\n", RTTI(renderPass));
}

static void			renderingSceneRender(SceneClass *scene)
{
  RenderingSceneClass*		this;
  RenderPassClass*		currentPass;
  int				i;

  this = RenderingSceneGetSingletonPtr();
  this->private.mScene = scene;
  i = 0;
  while (RenderPasses[i].name)
  {
    if (RenderPasses[i].activated)
    {
      printf("\033[0;040m\n");
      printf("%s renderpass\n", RenderPasses[i].name);
      printf("%s", charColors[i]);
      currentPass = RenderPasses[i].renderPassPtr;
      currentPass->apply(currentPass);
      GraphicsManagerGetSingletonPtr()\
        ->display(VOID(RenderingSceneGetSingletonPtr()->getCurrentState()));
      if (i == 0)
        break ;
    }
    ++i;
  }
  printf("\033[0;040m\n\nFinished rendering.\n");
  GraphicsManagerGetSingletonPtr()->end();
}

static GBufferClass*		renderingSceneGetGBuffer(void)
{
  return (RenderingSceneGetSingletonPtr()->private.mGBuffer);
}

static SceneClass*		renderingSceneGetCurrentScene(void)
{
  SceneClass			*scene;

  if (!(scene = RenderingSceneGetSingletonPtr()->private.mScene))
  {
    scene = RTEngineGetSingletonPtr()->getScene();
    RenderingSceneGetSingletonPtr()->private.mScene = scene;
  }
  return (scene);
}

static ColorBufferClass*	renderingSceneGetCurrentState(void)
{
  return (RenderingSceneGetSingletonPtr()->private.mCurrentState);
}

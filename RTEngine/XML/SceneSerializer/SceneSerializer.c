#include <string.h>

#include "RTEngine.h"
#include "SceneSerializer.h"
#include "GeometricObject.h"
#include "Utils.h"

#include "Phong.h"
#include "Matte.h"
#include "Emissive.h"
#include "Reflective.h"
#include "GlossyReflective.h"
#include "Transparent.h"
#include "Mesh.h"
#include "Checker.h"
#include "Noise.h"
#include "Bump.h"
#include "ImageTexture.h"

#include "Sphere.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Tore.h"
#include "Cone.h"
#include "Box.h"
#include "Rectangle.h"
#include "Disc.h"
#include "CubeTroue.h"

#include "PointLight.h"
#include "Ambient.h"
#include "EnvironmentLight.h"
#include "Directional.h"
#include "FakeSphericalLight.h"

#include "Pinhole.h"
#include "FishEye.h"
#include "Spherical.h"

#include "OversamplingRenderPass.h"
#include "LightRenderPass.h"
#include "ShadowRenderPass.h"
#include "AORenderPass.h"
#include "TransparencyRenderPass.h"
#include "ReflectionRenderPass.h"
#include "SSAORenderPass.h"
#include "InstRadiosityRenderPass.h"
#include "TextureRenderPass.h"
#include "BumpRenderPass.h"
#include "AreaLightRenderPass.h"
#include "PathTracingRenderPass.h"
#include "GammaRenderPass.h"
#include "ToneMappingRenderPass.h"
#include "NegativeRenderPass.h"

static void     SceneSerializerConstructor();
static void     SceneSerializerDestructor(SceneSerializerClass *self);
static MaterialClass	*sceneSerializerGetMaterialFromId(
                                  SceneSerializerClass *self,
                                  char *id);
static void	sceneSerializerPutError(SceneSerializerClass *self,
                                        xmlNodePtr node,
                                        char *format,
                                        ...);
static void	sceneSerializerSetStrsFromProp(SceneSerializerClass *self,
                                               xmlNodePtr node,
                                               ...);
static void	sceneSerializerUnserializeEngineProp(SceneSerializerClass *self,
                                                     xmlNodePtr node);
static void	sceneSerializerUnserializeSceneProp(SceneSerializerClass *self,
                                                    xmlNodePtr node);
static void	sceneSerializerHandleNewEntity(SceneSerializerClass *self,
                                              SceneEntityType type,
                                              void *entity);
static void	sceneSerializerCallUnserializeHandler(
                                  SceneSerializerClass *self,
                                                      xmlNodePtr node,
                                                      SceneEntityType type);
static void 	sceneSerializerUnserializeEntity(
                                  SceneSerializerClass *self,
                                  xmlNodePtr entitiesNode,
                                  SceneEntityType type);
static void	sceneSerializerUnserializeSceneEntities(
                                  SceneSerializerClass *self,
                                                        xmlNodePtr cur,
                                                        SceneEntityType type);
static void	sceneSerializerUnserialize(SceneSerializerClass *self);
static SceneClass	*sceneSerializerGetScene(SceneSerializerClass *self);

char	*entityName[4][2] = {
  { "material", "materials" },
  { "object", "objects" },
  { "light", "lights" },
  { "pass", "renderpass" }
};

UnserializerHandler handlerTab[] = {
  {eMaterial, "phong", &phongUnserialize},
  {eMaterial, "matte", &matteUnserialize},
  {eMaterial, "emissive", &emissiveUnserialize},
  {eMaterial, "reflective", &reflectiveUnserialize},
  {eMaterial, "glossy", &glossyReflectiveUnserialize},
  {eMaterial, "transparent", &transparentUnserialize},
  {eMaterial, "checker", &checkerUnserialize},
  {eMaterial, "noise", &noiseUnserialize},
  {eMaterial, "bump", &bumpUnserialize},
  {eMaterial, "image", &imageTextureUnserialize},

  {eObject, "sphere", &sphereUnserialize},
  {eObject, "plane", &planeUnserialize},
  {eObject, "cylinder", &cylinderUnserialize},
  {eObject, "tore", &toreUnserialize},
  {eObject, "cone", &coneUnserialize},
  {eObject, "box", &boxUnserialize},
  {eObject, "rectangle", &rectangleUnserialize},
  {eObject, "disc", &discUnserialize},
  {eObject, "mesh", &meshUnserialize},
  {eObject, "cube troue", &cubeTroueUnserialize},

  {eLight, "point", &pointLightUnserialize},
  {eLight, "ambient", &ambientUnserialize},
  {eLight, "directional", &directionalUnserialize},
  {eLight, "fake area", &fakeSphericalLightUnserialize},
  {eLight, "environment", &environmentLightUnserialize},

  {eCamera, "pinhole", &pinholeUnserialize},
  {eCamera, "fisheye", &fishEyeUnserialize},
  {eCamera, "spherical", &sphericalUnserialize},

  {eRenderPass, "oversampling", &oversamplingRenderPassUnserialize},
  {eRenderPass, "light", &lightRenderPassUnserialize},
  {eRenderPass, "shadow", &shadowRenderPassUnserialize},
  {eRenderPass, "ambient occlusion", &AORenderPassUnserialize},
  {eRenderPass, "transparency", &transparencyRenderPassUnserialize},
  {eRenderPass, "reflection", &reflectionRenderPassUnserialize},
  {eRenderPass, "area light", &areaLightRenderPassUnserialize},
  {eRenderPass, "ssao", &SSAORenderPassUnserialize},
  {eRenderPass, "instant radiosity", &instRadiosityRenderPassUnserialize},
  {eRenderPass, "texture", &textureRenderPassUnserialize},
  {eRenderPass, "bump", &bumpRenderPassUnserialize},
  {eRenderPass, "path tracing", &pathTracingRenderPassUnserialize},
  {eRenderPass, "gamma", &gammaRenderPassUnserialize},
  {eRenderPass, "tone mapping", &toneMappingRenderPassUnserialize},
  {eRenderPass, "negative", &negativeRenderPassUnserialize},

  {0, 0, 0}
};

static SceneSerializerClass       _descr = { /* SceneSerializerClass */
  { /* Class */
    sizeof(SceneSerializerClass),
    0, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
    &SceneSerializerConstructor,
    &SceneSerializerDestructor,
  },
  &sceneSerializerUnserialize,
  &sceneSerializerSetStrsFromProp,
  &sceneSerializerGetMaterialFromId,
  &sceneSerializerPutError,
  &sceneSerializerGetScene,
  { /* private */
    0, /* mXmlDocument */
    0, /* mScene */
    0, /* mRenderer */
    0 /* mMaterialsAssoc */ }
};

Class* SceneSerializer = (Class*) &_descr;

static void     SceneSerializerConstructor(
    SceneSerializerClass *self, va_list *list)
{
  self->private.mXmlDoc = new(XMLDocument, va_arg(*list, char*));
  self->private.mScene = new(Scene);
  RTEngineGetSingletonPtr()->addScene(self->private.mScene);
  self->private.mMaterialsAssoc = new(Vector);
}

static void     SceneSerializerDestructor(SceneSerializerClass *self)
{
  int		i;
  int		numMaterials;

  i = 0;
  numMaterials = self->private.mMaterialsAssoc\
                 ->size(self->private.mMaterialsAssoc);
  while (i < numMaterials)
  {
    if (MATASSOC(self->private.mMaterialsAssoc->mPtrs[i])->used == false)
      delete(self->private.mMaterialsAssoc->mPtrs[i]);
    i++;
  }
  delete(self->private.mXmlDoc);
}

static MaterialClass	*sceneSerializerGetMaterialFromId(
    SceneSerializerClass *self,
                                                 char *id)
{
  int		i;
  int		size;

  i = 0;
  size = self->private.mMaterialsAssoc->size(self->private.mMaterialsAssoc);
  while (i < size)
  {
    if (strcmp(id, MATASSOC(self->private.mMaterialsAssoc->mPtrs[i])->id) == 0)
    {
      MATASSOC(self->private.mMaterialsAssoc->mPtrs[i])->used = true;
      return (MATASSOC(self->private.mMaterialsAssoc->mPtrs[i])->material);
    }
    i++;
  }
  return (0);
}

static void	sceneSerializerPutError(SceneSerializerClass *self,
                                        xmlNodePtr node,
                                        char *format,
                                        ...)
{
  va_list	list;

  va_start(list, format);
  self->private.mXmlDoc->vPutError(self->private.mXmlDoc, node, format, &list);
  va_end(list);
}

static void	sceneSerializerSetStrsFromProp(SceneSerializerClass *self,
                                               xmlNodePtr node,
                                               ...)
{
  va_list	list;

  va_start(list, node);
  self->private.mXmlDoc->vSetStrsFromProp(self->private.mXmlDoc, node, &list);
  va_end(list);
}

/* vvv */

static void	sceneSerializerUnserializeEngineProp(
				SceneSerializerClass *self,
                                                     xmlNodePtr node)
{
  char		*strSize;
  char		*strDepth;
  char		*strDistAtt;
  char		*strSamplerShared;
  char		*strSampler;
  int		depth;

  strSize = (char*) xmlGetProp(node, XMLSTR("size"));
  strDistAtt = (char*) xmlGetProp(node, XMLSTR("attenuation"));
  strSamplerShared = (char*) xmlGetProp(node, XMLSTR("samplerSharing"));
  strSampler = (char*) xmlGetProp(node, XMLSTR("sampler"));
  if ((strDepth = (char*) xmlGetProp(node, XMLSTR("depth"))))
  {
    if (setIntFromStr(strDepth, &depth) != 0)
      exit(1);
    RTEngineGetSingletonPtr()->setSceneDepth(depth);
  }
  if (strSize)
    RTEngineGetSingletonPtr()->setSceneSize(strSize);
  if (strDistAtt)
    RTEngineGetSingletonPtr()->setLightAttenuation(strDistAtt);
  if (strSamplerShared)
    RTEngineGetSingletonPtr()->setSamplerShared(strSamplerShared);
  if (strSampler)
    RTEngineGetSingletonPtr()->setDefaultSampler(strSampler);
}

static void	sceneSerializerUnserializeSceneProp(SceneSerializerClass *self,
                                                    xmlNodePtr node)
{
  xmlNodePtr	scenePropNode, cameraNode, renderingNode;

  scenePropNode = self->private.mXmlDoc\
                 ->getNodeByName(self->private.mXmlDoc, node, "image");
  if (scenePropNode == 0)
    self->putError(self, node, "The description does \
        not specify image properties.\n");
  node = scenePropNode->xmlChildrenNode;
  cameraNode = self->private.mXmlDoc->getNodeByName(self->private.mXmlDoc,
                                                    node, "camera");
  renderingNode = self->private.mXmlDoc->getNodeByName(self->private.mXmlDoc,
                                                    node, "rendering");
  if (cameraNode == 0)
    self->putError(self, node, "Missing properties in image description.\n");
  sceneSerializerCallUnserializeHandler(self, cameraNode, eCamera);
  if (renderingNode)
    sceneSerializerUnserializeEngineProp(self, renderingNode);
  RTEngineGetSingletonPtr()->init();
  self->private.mRenderer = RenderingSceneGetSingletonPtr();
}

static void	sceneSerializerHandleNewEntity(SceneSerializerClass *self,
                                              SceneEntityType type,
                                              void *entity)

{
  if (type == eObject)
    self->private.mScene->addObject(self->private.mScene, entity);
  else if (type == eLight)
    self->private.mScene->addLight(self->private.mScene, entity);
  else if (type == eMaterial)
    self->private.mMaterialsAssoc->push_back(self->private.mMaterialsAssoc,
        entity);
  else if (type == eCamera)
    self->private.mScene->setCamera(self->private.mScene, entity);
  else if (type == eRenderPass)
    self->private.mRenderer->addRenderPass(entity);
}

static void	sceneSerializerCallUnserializeHandler(
                                            SceneSerializerClass *self,
                                                      xmlNodePtr node,
                                                      SceneEntityType type)
{
  int		i;
  xmlChar	*strType;
  void		*newEntity;

  if (!(strType = xmlGetProp(node, XMLSTR("type"))))
    self->putError(self, node, "Entity does not have a type.\n");
  i = 0;
  while (handlerTab[i].name)
  {
    if (handlerTab[i].type == type
        && xmlStrcmp(strType, XMLSTR(handlerTab[i].name)) == 0)
    {
      if ((newEntity = handlerTab[i].handler(self, node)) == 0)
        self->putError(self, node, "Cannot create entity.\n");
      sceneSerializerHandleNewEntity(self, type, newEntity);
      return ;
    }
    i++;
  }
  raise("Could not find handler for entity type: %d\n", type);
}

static void 	sceneSerializerUnserializeEntity(SceneSerializerClass *self,
                                  xmlNodePtr entitiesNode,
                                  SceneEntityType type)
{
  xmlNodePtr	cur;

  cur = entitiesNode;
  while (cur != 0)
  {
    if (xmlStrcmp(cur->name, XMLSTR(entityName[type][CHILD_NAME])) != 0)
    {
      if (xmlStrcmp(cur->name, XMLSTR("text")) != 0
            && xmlStrcmp(cur->name, XMLSTR("comment")) != 0)
        self->putError(self, cur, "There is no %s. [%s]\n",
            entityName[type][CHILD_NAME], cur->name);
    }
    else
      sceneSerializerCallUnserializeHandler(self, cur, type);
    cur = cur->next;
  }
}

static void	sceneSerializerUnserializeSceneEntities(
                                            SceneSerializerClass *self,
                                                        xmlNodePtr cur,
                                                        SceneEntityType type)
{
  xmlNodePtr	entitiesNode;

  entitiesNode = self->private.mXmlDoc\
                 ->getNodeByName(self->private.mXmlDoc, cur,
                     entityName[type][PARENT_NAME]);
  if (entitiesNode == 0 && type == eRenderPass)
  {
    printf("No render pass\n");
    return ;
  }
  if (entitiesNode == 0)
    raise("The scene does not contain any %s.\n",
        entityName[type][CHILD_NAME]);
  sceneSerializerUnserializeEntity(self, entitiesNode->xmlChildrenNode, type);
}

static void	sceneSerializerUnserialize(SceneSerializerClass *self)
{
  xmlNodePtr	cur;

  self->private.mXmlDoc->init(self->private.mXmlDoc);
  cur = self->private.mXmlDoc->getRoot(self->private.mXmlDoc);
  if (xmlStrcmp(cur->name, XMLSTR(SCENE_NODE)) != 0)
    raise("Not a scene description.\n");
  cur = cur->xmlChildrenNode;
  sceneSerializerUnserializeSceneProp(self, cur);
  sceneSerializerUnserializeSceneEntities(self, cur, eRenderPass);
  sceneSerializerUnserializeSceneEntities(self, cur, eMaterial);
  sceneSerializerUnserializeSceneEntities(self, cur, eObject);
  sceneSerializerUnserializeSceneEntities(self, cur, eLight);
}

/* ^^^ */

static SceneClass	*sceneSerializerGetScene(SceneSerializerClass *self)
{
  return (self->private.mScene);
}

#ifndef __SCENESERIALIZER_H__
# define __SCENESERIALIZER_H__

# include "OOC.h"
# include "Vector.h"
# include "XMLDocument.h"
# include "Scene.h"
# include "RenderingScene.h"

# define SCENE_NODE		"scene"

# define CHILD_NAME			0
# define PARENT_NAME			1

typedef enum				SceneEntityType
{
  eMaterial = 0,
  eObject = 1,
  eLight = 2,
  eRenderPass = 3,
  eCamera = 4,
} 					SceneEntityType;

typedef struct SceneSerializerClass	SceneSerializerClass;
struct					SceneSerializerClass
{
  /* SuperClass : */
  Class				__base__;
  /* public : */
  void				(*unserialize)(SceneSerializerClass *self);
  void				(*setStrsFromProp)(SceneSerializerClass *self,
                                                   xmlNodePtr, ...);
  MaterialClass*		(*getMaterialFromId)(SceneSerializerClass *self,
                                                      char *id);
  void				(*putError)(SceneSerializerClass *self,
                                           xmlNodePtr node, char *format, ...);
  SceneClass			*(*getScene)(SceneSerializerClass *self);
  /* private : */
  struct
  {
    XMLDocumentClass		*mXmlDoc;
    SceneClass			*mScene;
    RenderingSceneClass		*mRenderer;
    VectorClass			*mMaterialsAssoc;
  } private;
};

extern Class			*SceneSerializer;
extern char			*entityName[4][2];

# define MATASSOC(x)		((MaterialIdAssoc*) (x))

typedef struct	MaterialIdAssoc		MaterialIdAssoc;
struct					MaterialIdAssoc
{
  bool					used;
  char					*id;
  MaterialClass				*material;
};

typedef struct UnserializerHandler	UnserializerHandler;
struct 					UnserializerHandler
{
  SceneEntityType			type;
  char					*name;
  void				*(*handler)(SceneSerializerClass *serializer,
                                                   xmlNodePtr node);
};

extern UnserializerHandler  	handlerTab[];

#endif /* !__SCENESERIALIZER_H__ */

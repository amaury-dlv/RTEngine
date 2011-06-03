#ifndef __CLUSTERCLIENT_H__
# define __CLUSTERCLIENT_H__

# include <stdio.h>

# include "OOC.h"
# include "Cluster.h"
# include "RenderingScene.h"
# include "SceneSerializer.h"
# include "RenderPass.h"

# define	CLIENT(x)		((ClusterClientClass *) (x))
# define	CLUSTERCLIENT_BUFSIZE	128

typedef struct ClusterClientClass	ClusterClientClass;
struct					ClusterClientClass
{
  /* SuperClass : */
  ClusterClass				__base__;
  /* public : */
  void					(*processColumn)(int col);
  void					(*readFile)(void);
  void					(*setRenderPass)(void);
  void					(*connect)();
  /* private : */
  struct
  {
    SceneSerializerClass		*mSceneSerializer;
    RenderPassClass			*mRenderPass;
    char				*mHost;
    int					mXmlSize;
    char				mXmlFile[10];
    int					mXmlFd;
    int					mSockFd;
    bool				mIsVerbose;
  } private;
};

extern Class				*ClusterClient;

#endif /* !__CLUSTERCLIENT_H__ */

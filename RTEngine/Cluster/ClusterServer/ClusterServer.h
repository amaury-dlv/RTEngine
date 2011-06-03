#ifndef __CLUSTERSERVER_H__
# define __CLUSTERSERVER_H__

# include <stdio.h>

# include "OOC.h"
# include "Cluster.h"

# define SERVER(x)	((ClusterServerClass*) (x))

# define MAX_CLIENTS	1024

typedef enum RenderingState	RenderingState;
enum				RenderingState
{
  eIdle,
  eWorking,
  eDone
};

typedef struct ClusterServerClass	ClusterServerClass;
struct                          	ClusterServerClass
{
  /* SuperClass : */
  ClusterClass			__base__;
  /* public : */
  void				(*launchClient)(void);
  void				(*initXmlFile)(void);
  void				(*handleNewConnection)(void);
  void				(*initClient)(int sockClient);
  void				(*handleData)(int clientSock);
  void				(*giveWorkToClient)(int clientSock);
  void				(*removeClient)(int clientSock);
  void				(*handleClientColumnData)(int clientSock);
  void				(*updateScreen)(void);
  void				(*end)(void);
  /* private : */
  struct
  {
    char			*mStartedOn;
    time_t			mStart;
    char			*mXmlFile;
    int				mXmlFd;
    int				mXmlSize;
    RenderPassClass		*mRenderPass;
    int				mWidth;
    int				mHeight;
    RenderingState		*mColumnsState;
    int				mClientColumn[MAX_CLIENTS];
    char			*mClientIp[MAX_CLIENTS];
    time_t			mClientArrival[MAX_CLIENTS];
    int				mClientNumCompleted[MAX_CLIENTS];
    int				mNumCompleted;
    fd_set			mMasterSet;
    int				mListener;
    int				mFdMax;
    Color			*mBuf;
  } private;
};

extern Class*		ClusterServer;

#endif /* !__CLUSTERSERVER_H__ */

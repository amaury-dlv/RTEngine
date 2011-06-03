#ifndef __CLUSTER_H__
# define __CLUSTER_H__

# include <stdio.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <sys/select.h>
# include <sys/time.h>
# include <unistd.h>
# include <netdb.h>
# include <arpa/inet.h>
# include <netinet/in.h>
# include <fcntl.h>

# include "OOC.h"

# define PORT		"31313"

# define CLUSTER(x)			((ClusterClass*) (x))

typedef struct ClusterClass		ClusterClass;
struct                          	ClusterClass
{
  /* SuperClass : */
  Class				__base__;
  /* public : */
  void				(*init)();
  void				(*run)();
  void				(*setAddrInfo)();
  void				(*sendBuf)(int fd, void *buf, int len);
  void				(*sendFile)(int fd, int fileFd);
  int				(*recvBuf)(int fd, void *buf, int len);
  /* private : */
  struct
  {
    ColorBufferClass		*mCurrentState;
  } protected;
};

ClusterClass*		ClusterGetSingletonPtr(void);

extern Class*		Cluster;

#endif /* !__CLUSTER_H__ */

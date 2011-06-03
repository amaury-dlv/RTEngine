#include "RTEngine.h"
#include "Cluster.h"

static void     ClusterConstructor(ClusterClass *self, va_list *list);
static void     ClusterDestructor();
static void	clusterSendBuf(int fd, void *buf, int len);
static void	clusterSendFile(int fd, int fileFd);
static int 	clusterRecvBuf(int fd, void *buf, int len);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static ClusterClass       _descr = { /* ClusterClass */
  { /* Class */
    sizeof(ClusterClass),
    0, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
    &ClusterConstructor,
    &ClusterDestructor,
  },
  0, /* init */
  0, /* run */
  0, /* setAddrInfo */
  &clusterSendBuf,
  &clusterSendFile,
  &clusterRecvBuf,
  {
    0 /* mCurrentState */
  }
};

Class *Cluster = (Class*) &_descr;

static ClusterClass *sSingleton = 0;

static void     ClusterConstructor(ClusterClass *self,
                                   va_list *list)
{
  if (sSingleton != 0)
    raise("Cannot new a singleton twice : Cluster\n");
  sSingleton = self;
  self->sendBuf = &clusterSendBuf;
  self->sendFile = &clusterSendFile;
  self->recvBuf = &clusterRecvBuf;
}

static void     ClusterDestructor()
{
}

ClusterClass*		ClusterGetSingletonPtr(void)
{
  return (sSingleton);
}

static void			clusterSendBuf(int fd, void *buf, int len)
{
  int			total;
  int			nBytes;

  total = 0;
  while (total != len)
  {
    nBytes = send(fd, buf + total, len - total, 0);
    if (nBytes < 0)
      return ;
    total += nBytes;
  }
}

static void			clusterSendFile(int fd, int fileFd)
{
  ClusterClass		*self;
  int			nBytes;
  char			buf[1024];

  self = ClusterGetSingletonPtr();
  if ((lseek(fileFd, 0, SEEK_SET)) == -1)
    raise("lseek failure.\n");
  while ((nBytes = read(fileFd, &buf, sizeof(buf))))
    self->sendBuf(fd, buf, nBytes);
  if (nBytes == -1)
    raise("read failure.\n");
}

static int			clusterRecvBuf(int fd, void *buf, int len)
{
  int				total;
  int				nBytes;

  total = 0;
  while (total != len)
  {
    nBytes = recv(fd, buf + total, len - total, 0);
    if (total == 0 && nBytes == 0)
      return (0);
    if (nBytes == -1)
    {
      perror("recv");
      return (-1);
    }
    total += nBytes;
  }
  return (total);
}

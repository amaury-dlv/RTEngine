#include <stdlib.h>
#include <string.h>

#include "RTEngine.h"
#include "ClusterClient.h"
#include "OversamplingRenderPass.h"
#include "PathTracingRenderPass.h"
#include "Utils.h"
#include "Maths.h"
#include "OptionsManager.h"

static void     ClusterClientConstructor();
static void	ClusterClientDestructor();
static void	*clusterClientGetInAddr(struct sockaddr *sa);
static void	clusterClientPrint(struct addrinfo *p);
static void	clusterClientConnect(struct addrinfo *ai, int *sockFd);
static void	clusterClientSetAddrInfo(struct addrinfo **ai, char *host);
static void	clusterClientInit(void);
static void	clusterClientRun(void);
static void	clusterClientReadFile(void);
static void	clusterClientSetRenderPass(void);
static void	clusterClientProcessColumn(int col);

static ClusterClientClass       _descr = { /* ClusterClientClass */
  { /* Cluster */
    { /* Class */
      sizeof(ClusterClientClass),
      0, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
      &ClusterClientConstructor,
      &ClusterClientDestructor,
    },
    0, /* init */
    0, /* run */
    0, /* setAddrInfo */
    0, /* sendBuf */
    0, /* sendFile */
    0, /* recvBuf */
    { /* protected */
      0 /* mCurrentState */
    }
  },
  &clusterClientProcessColumn,
  &clusterClientReadFile,
  &clusterClientSetRenderPass,
  &clusterClientConnect,
  { /* private */
    0, /* mSceneSerializer */
    0, /* mRenderPass */
    0, /* mHost */
    0, /* mXmlSize */
    "rt.XXXXXX", /* mXmlFile */
    0, /* mXmlFd */
    0, /* mSockFd */
    1 /* mIsVerbose */
  }
};

Class *ClusterClient = (Class*) &_descr;

static void     ClusterClientConstructor(ClusterClientClass *self,
    					 va_list *list)
{
  char		*is_verbose;

  Cluster->__constructor__(self, list);
  CLUSTER(self)->init = &clusterClientInit;
  CLUSTER(self)->run = &clusterClientRun;
  CLUSTER(self)->setAddrInfo = &clusterClientSetAddrInfo;
  PRIV(mHost) = va_arg(*list, char*);
  CLUSTER(self)->init();
  is_verbose = OptionsManagerGetSingletonPtr()->getOptionValue("verbose");
  if (is_verbose && !strcmp(is_verbose, "no"))
    PRIV(mIsVerbose) = false;
}

static void     		ClusterClientDestructor()
{
}

static void 			*clusterClientGetInAddr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET)
    return (&(((struct sockaddr_in*)sa)->sin_addr));
  return (&(((struct sockaddr_in6*)sa)->sin6_addr));
}

static void			clusterClientPrint(struct addrinfo *p)
{
  char				b[INET6_ADDRSTRLEN];

  inet_ntop(p->ai_family, clusterClientGetInAddr(p->ai_addr), b, sizeof(b));
  fprintf(stderr, "Connecting to %s\n", b);
}

static void			clusterClientConnect(struct addrinfo *ai,
                                                     int *sockFd)
{
  struct addrinfo		*p;

  p = ai;
  do
  {
    *sockFd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (*sockFd < 0)
      continue;
    setsockopt(*sockFd, SOL_SOCKET, SO_REUSEADDR, (int[]){1}, sizeof(int));
    if (connect(*sockFd, p->ai_addr, p->ai_addrlen) < 0)
    {
      close(*sockFd);
      continue;
    }
    break;
  } while ((p = p->ai_next) != NULL);
  if (p == NULL)
    raise("connect failure.\n");
  clusterClientPrint(p);
  freeaddrinfo(ai);
}

static void			clusterClientSetAddrInfo(struct addrinfo **ai,
                                                         char *host)
{
  struct addrinfo		hints;
  int				rv;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  if ((rv = getaddrinfo(host, PORT, &hints, ai)) != 0)
    raise("%s : %s:%s\n", gai_strerror(rv), host, PORT);
}

static void			clusterClientInit(void)
{
  struct addrinfo		*ai;
  ClusterClientClass		*self;

  self = CLIENT(ClusterGetSingletonPtr());
  CLUSTER(self)->setAddrInfo(&ai, PRIV(mHost));
  self->connect(ai, &PRIV(mSockFd));
}

static void		clusterClientRun(void)
{
  static int		count;
  int			col;
  ClusterClientClass	*self;

  self = CLIENT(ClusterGetSingletonPtr());
  self->readFile();
  self->setRenderPass();
  while (1)
  {
    if (CLUSTER(self)->recvBuf(PRIV(mSockFd), &col, sizeof(col)) == -1)
      raise("Nothing more to process");
    if (PRIV(mIsVerbose))
    {
      printf("\rProcessing column %d [processed: %d]...", col, count);
      fflush(stdout);
    }
    CLUSTER(self)->protected.mCurrentState = RenderingSceneGetSingletonPtr()\
      ->getCurrentState();
    self->processColumn(col);
    ++count;
  }
}

static void			clusterClientReadFile(void)
{
  ClusterClientClass		*self;
  char				buf[CLUSTERCLIENT_BUFSIZE];
  int				r;
  int				rtmp;

  self = CLIENT(ClusterGetSingletonPtr());
  if (((CLUSTER(self)->recvBuf(PRIV(mSockFd),
         &PRIV(mXmlSize), sizeof(PRIV(mXmlSize)))) == -1)
      || (PRIV(mXmlFd) = mkstemp(PRIV(mXmlFile))) == -1)
    raise("Unable to process request\n");
  r = 0;
  while (r < PRIV(mXmlSize)
      && (rtmp = CLUSTER(self)->recvBuf(PRIV(mSockFd), buf,
          MIN(CLUSTERCLIENT_BUFSIZE, (PRIV(mXmlSize) - r)))) != -1)
  {
    if (write(PRIV(mXmlFd), buf, rtmp) != rtmp)
      raise("Unable to write temporary scene description\n");
    r += rtmp;
  }
  if (close(PRIV(mXmlFd)) == -1)
    perror("close");
  PRIV(mSceneSerializer) = new(SceneSerializer, PRIV(mXmlFile));
  PRIV(mSceneSerializer)->unserialize(PRIV(mSceneSerializer));
  if (unlink(PRIV(mXmlFile)) == -1)
    perror("unlink");
}

static void			clusterClientSetRenderPass(void)
{
  ClusterClientClass		*self;
  int				type;

  self = CLIENT(ClusterGetSingletonPtr());
  if ((CLUSTER(self)->recvBuf(PRIV(mSockFd), &type, sizeof(type))) == -1)
    raise("Unable to process request\n");
  PRIV(mRenderPass) = RenderingSceneGetSingletonPtr()->getRenderPass(type);
  if (RTTI(PRIV(mRenderPass)) != RTTI(PathTracingRenderPass)
      && RTTI(PRIV(mRenderPass)) != RTTI(OversamplingRenderPass))
    raise("Non-valid render pass selected\n");
  CLUSTER(self)->protected.mCurrentState = RenderingSceneGetSingletonPtr()\
    ->getCurrentState();
  if (PRIV(mIsVerbose))
    printf("Proceding...\n");
  PRIV(mRenderPass)->init(PRIV(mRenderPass));
}

static void			clusterClientProcessColumn(int col)
{
  ClusterClientClass		*self;
  int				height;
  int				i;

  self = CLIENT(ClusterGetSingletonPtr());
  height = RTEngineGetSingletonPtr()->getSceneHeight();
#ifndef USE_THREADS
  i = -1;
  while (++i < height)
#else
# pragma omp parallel for private(i) schedule(dynamic, 4)
  for (i = 0; i < height; i++)
#endif
    PRIV(mRenderPass)->applyPixel(PRIV(mRenderPass), col, i);
  GraphicsManagerGetSingletonPtr()\
    ->display(IMAGEBUF(RenderingSceneGetSingletonPtr()->getCurrentState()));
  PRIV(mRenderPass)->updateCurrentState(PRIV(mRenderPass));
  i = -1;
  while (++i < height)
    CLUSTER(self)->sendBuf(PRIV(mSockFd),
      CLUSTER(self)->protected.mCurrentState->getColorPtr(
        CLUSTER(self)->protected.mCurrentState,
	 col + i * RTEngineGetSingletonPtr()->getSceneWidth()),
      sizeof(Color));
}

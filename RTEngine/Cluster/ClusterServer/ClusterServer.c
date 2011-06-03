#include <string.h>
#include <time.h>

#include "RTEngine.h"
#include "ClusterServer.h"
#include "PathTracingRenderPass.h"
#include "Utils.h"
#include "OptionsManager.h"

static void     ClusterServerConstructor();
static void     ClusterServerDestructor();
static void	clusterServerLaunchClient(void);
static void	clusterServerInitXmlFile(void);
static void 	*clusterServerGetInAddr(struct sockaddr *sa);
static void	clusterServerListen();
static void	clusterServerBind(struct addrinfo *ai,
                                  int *sockFd);
static void	clusterServerSetAddrInfo(struct addrinfo **ai);
static void	clusterServerInit(void);
static void	clusterServerUpdateReadFd(fd_set *set);
static void	clusterServerRun(void);
static void	clusterServerSendSceneFile(int sockClient);
static void	clusterServerInitClient(int sockClient);
static void	clusterServerHandleNewConnection(void);
static void	clusterServerHandleData(int clientSock);
static void	clusterServerGiveWorkToClient(int clientSock);
static void	clusterServerPrintBar(char a, int na);
static void	clusterServerPrintProgress(int current, int total);
static void	clusterServerUpdateScreen(void);
static void	clusterServerHandleClientColumnData(int clientSock);
static void	clusterServerRemoveClient(int clientSock);
static void	clusterServerEnd();
static void	clusterServerRunInit(fd_set *readFds);

/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static ClusterServerClass       _descr = { /* ClusterServerClass */
  { /* Cluster */
    { /* Class */
      sizeof(ClusterServerClass),
      0, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
      &ClusterServerConstructor,
      &ClusterServerDestructor,
    },
    &clusterServerInit,
    &clusterServerRun,
    &clusterServerSetAddrInfo,
    0, /* sendBuf */
    0, /* sendFile */
    0, /* recvBuf */
    { /* protected */
      0 /* mCurrentState */
    }
  },
  &clusterServerLaunchClient,
  &clusterServerInitXmlFile,
  &clusterServerHandleNewConnection,
  &clusterServerInitClient,
  &clusterServerHandleData,
  &clusterServerGiveWorkToClient,
  &clusterServerRemoveClient,
  &clusterServerHandleClientColumnData,
  &clusterServerUpdateScreen,
  &clusterServerEnd,
  { /* private */
    0, /* mStartedOn */
    0, /* mStart */
    0, /* mXmlFile */
    0, /* mXmlFd */
    0, /* mXmlSize */
    0, /* mRenderPass */
    0, /* mWidth */
    0, /* mHeight */
    0, /* mColumnState */
    {0}, /* mClientColumn */
    {0}, /* mClientIp */
    {0}, /* mClientArrival */
    {0}, /* mClientNumCompleted */
    0, /* mNumCompleted */
    {{0}}, /* mMasterSet */
    0, /* mListener */
    0, /* mFdMax */
    0, /* mBuf */
  }
};

Class *ClusterServer = (Class*) &_descr;

static void     ClusterServerConstructor(ClusterServerClass *self,
    					 va_list *list)
{
  int		i;
  time_t	timer;

  Cluster->__constructor__(self, list);
  PRIV(mRenderPass) = va_arg(*list, RenderPassClass*);
  if (PRIV(mRenderPass) == 0)
    PRIV(mRenderPass) = RENDERPASS(PathTracingRenderPass);
  PRIV(mXmlFile) = va_arg(*list, char*);
  PRIV(mWidth) = RTEngineGetSingletonPtr()->getSceneWidth();
  PRIV(mHeight) = RTEngineGetSingletonPtr()->getSceneHeight();
  PRIV(mColumnsState) = xmalloc(sizeof(*PRIV(mColumnsState)) * PRIV(mWidth));
  PRIV(mBuf) = xmalloc(sizeof(*PRIV(mBuf)) * PRIV(mHeight));
  time(&timer);
  PRIV(mStartedOn) = ctime(&timer);
  PRIV(mStart) = time(NULL);
  i = 0;
  while (i < PRIV(mWidth))
    PRIV(mColumnsState)[i++] = eIdle;
  i = 0;
  while (i < MAX_CLIENTS)
    PRIV(mClientColumn)[i++] = -1;
  FD_ZERO(&PRIV(mMasterSet));
  self->initXmlFile();
  CLUSTER(self)->init();
}

static void     		ClusterServerDestructor()
{
}

static void		clusterServerLaunchClient(void)
{
  int			ret;
  char			*opt;
  char			*path;

  if ((opt = OptionsManagerGetSingletonPtr()\
      ->getOptionValue("Fork")) && strcasecmp("no", opt) == 0)
    return ;
  printf("Forking a client.\n");
  ret = fork();
  if (ret == -1)
    perror("fork");
  else if (ret > 0)
    return ;
  if ((ret = open("/dev/null", O_WRONLY)) != -1)
  {
    if (dup2(ret, STDOUT_FILENO) == -1)
      perror("dup2");
    else
      close(ret);
  }
  path = OptionsManagerGetSingletonPtr()\
         ->getArg(0);
  execlp(path, path, "Cluster:127.0.0.1", "Verbose:no", "Display:none", NULL);
  raise("execlp failure.\n");
}

static void		clusterServerInitXmlFile(void)
{
  ClusterServerClass	*self;

  self = SERVER(ClusterGetSingletonPtr());
  if (!(PRIV(mXmlFd) = open(PRIV(mXmlFile), O_RDONLY)))
    raise("Cannot open file.\n");
  if ((PRIV(mXmlSize) = lseek(PRIV(mXmlFd), 0, SEEK_END)) == -1)
    raise("lseek failure.\n");
}

static void 	*clusterServerGetInAddr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET)
    return (&(((struct sockaddr_in*)sa)->sin_addr));
  return (&(((struct sockaddr_in6*)sa)->sin6_addr));
}

static void			clusterServerListen()
{
  ClusterServerClass		*self;

  self = SERVER(ClusterGetSingletonPtr());
  if (listen(PRIV(mListener), 10) == -1)
  {
    perror("listen");
    exit(3);
  }
  FD_SET(PRIV(mListener), &PRIV(mMasterSet));
  PRIV(mFdMax) = PRIV(mListener);
}

static void			clusterServerBind(struct addrinfo *ai,
                                                  int *sockFd)
{
  struct addrinfo		*p;

  p = ai;
  do
  {
    *sockFd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (*sockFd < 0)
      continue;
    if (setsockopt(*sockFd, SOL_SOCKET, SO_REUSEADDR, (int[]){1},
          sizeof(int)) == -1)
      perror("setsockopt");
    if (bind(*sockFd, p->ai_addr, p->ai_addrlen) < 0)
    {
      close(*sockFd);
      continue;
    }
    break;
  } while ((p = p->ai_next) != NULL);
  freeaddrinfo(ai);
  if (p == NULL)
    raise("bind failure.\n");
}

static void			clusterServerSetAddrInfo(struct addrinfo **ai)
{
  struct addrinfo		hints;
  int				rv;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  if ((rv = getaddrinfo(NULL, PORT, &hints, ai)) != 0)
    raise("%s\n", gai_strerror(rv));
}

static void			clusterServerInit(void)
{
  ClusterServerClass		*self;
  struct addrinfo 		*ai;

  self = SERVER(ClusterGetSingletonPtr());
  CLUSTER(self)->setAddrInfo(&ai);
  clusterServerBind(ai, &PRIV(mListener));
  clusterServerListen();
}

static void			clusterServerUpdateReadFd(fd_set *set)
{
  ClusterServerClass		*self;

  self = SERVER(ClusterGetSingletonPtr());
  *set = PRIV(mMasterSet);
  if (select(PRIV(mFdMax) + 1, set, NULL, NULL, NULL) == -1)
  {
    perror("select");
    exit(4);
  }
}

static void			clusterServerRunInit(fd_set *readFds)
{
  clusterServerLaunchClient();
  printf("Now running...\n");
  FD_ZERO(readFds);
}

static void			clusterServerRun(void)
{
  ClusterServerClass		*self;
  fd_set			readFds;
  int				i;

  clusterServerRunInit(&readFds);
  self = SERVER(ClusterGetSingletonPtr());
  CLUSTER(self)->protected.mCurrentState
    = RenderingSceneGetSingletonPtr()->getCurrentState();
  while (PRIV(mNumCompleted) != PRIV(mWidth))
  {
    clusterServerUpdateReadFd(&readFds);
    i = -1;
    while (++i <= PRIV(mFdMax))
      if (FD_ISSET(i, &readFds))
      {
        if (i == PRIV(mListener))
          self->handleNewConnection();
        else
        {
          self->handleData(i);
          self->updateScreen();
        }
      }
  }
  self->end();
}

static void			clusterServerSendSceneFile(int sockClient)
{
  ClusterServerClass		*self;
  int				rtti;

  self = SERVER(ClusterGetSingletonPtr());
  rtti = RTTI(PRIV(mRenderPass));
  CLUSTER(self)->sendBuf(sockClient,
      &PRIV(mXmlSize),
      sizeof(PRIV(mXmlSize)));
  CLUSTER(self)->sendFile(sockClient, PRIV(mXmlFd));
  CLUSTER(self)->sendBuf(sockClient,
      &rtti,
      sizeof(int));
}

static void			clusterServerInitClient(int sockClient)
{
  ClusterServerClass		*self;

  self = SERVER(ClusterGetSingletonPtr());
  if (lseek(PRIV(mXmlFd), 0, SEEK_SET) == -1)
    raise("lseek failure.\n");
  clusterServerSendSceneFile(sockClient);
}

static void			clusterServerHandleNewConnection(void)
{
  ClusterServerClass		*self;
  int				newFd;
  struct sockaddr_storage 	remoteAddr;
  char 				remoteIP[INET6_ADDRSTRLEN], *ipStr;
  socklen_t			addrLen;

  self = SERVER(ClusterGetSingletonPtr());
  addrLen = sizeof(remoteAddr);
  newFd = accept(PRIV(mListener), (struct sockaddr *)&remoteAddr, &addrLen);
  if (newFd == -1) {
    perror("accept");
    return ;
  }
  ipStr = (void*)inet_ntop(remoteAddr.ss_family,
        clusterServerGetInAddr((struct sockaddr*) &remoteAddr),
        remoteIP, INET6_ADDRSTRLEN);
  printf("Connection from %s on socket %d\n", ipStr, newFd);
  FD_SET(newFd, &PRIV(mMasterSet));
  PRIV(mClientIp)[newFd] = strdup(ipStr);
  PRIV(mClientArrival)[newFd] = time(NULL);
  self->initClient(newFd);
  if (newFd > PRIV(mFdMax)) PRIV(mFdMax) = newFd;
  self->giveWorkToClient(newFd);
}

static void			clusterServerHandleData(int clientSock)
{
  ClusterServerClass		*self;
  int				nBytes;

  self = SERVER(ClusterGetSingletonPtr());
  if ((nBytes = CLUSTER(self)->recvBuf(clientSock,
          PRIV(mBuf),
          sizeof(*PRIV(mBuf)) * PRIV(mHeight))) <= 0)
    self->removeClient(clientSock);
  else
    self->handleClientColumnData(clientSock);
}

static void			clusterServerGiveWorkToClient(int clientSock)
{
  ClusterServerClass		*self;
  int				i;

  i = 0;
  self = SERVER(ClusterGetSingletonPtr());
  while (i < PRIV(mWidth))
  {
    if (PRIV(mColumnsState)[i] == eIdle)
    {
      CLUSTER(self)->sendBuf(clientSock, &i, sizeof(i));
      PRIV(mClientColumn)[clientSock] = i;
      PRIV(mColumnsState)[i] = eWorking;
      return ;
    }
    i++;
  }
}

static void	clusterServerPrintBar(char a, int na)
{
  int		i;

  i = 0;
  while (i < na)
  {
    putchar(a);
    i++;
  }
  putchar('>');
  i = 0;
  while (i < 60 - na)
  {
    putchar(' ');
    i++;
  }
}

static void	clusterServerPrintProgress(int current, int total)
{
  static int	done = 0;
  static double	last_nb = -1;
  double	nb;

  printf("\n");
  nb = (((double)current) / total) * 100.0;
  if (nb > last_nb || 1)
  {
    putchar('[');
    clusterServerPrintBar('=', (int)(nb * 0.6));
    printf("] %.2f%%\r", nb);
  }
  last_nb = nb;
  done = ((last_nb == 100) ? 1 : 0);
  printf("\n");
}

static void		clusterServerUpdateScreen(void)
{
  ClusterServerClass	*self;
  int			i, col;
  int			numSecs;

  i = 0;
  system("clear");
  self = SERVER(ClusterGetSingletonPtr());
  printf("# RTEngine: started on %s\n Client list:\n\n", PRIV(mStartedOn));
  while (i < PRIV(mFdMax) + 1)
  {
    if ((col = PRIV(mClientColumn)[i]) >= 0)
    {
      numSecs = difftime(time(NULL), PRIV(mClientArrival)[i]);
      printf("%d [%s]: processing column %d\tcomputed %d columns\
\t[%dmin %dsecs] speed: %.1f col/sec\n",
          i, PRIV(mClientIp)[i], col,
          PRIV(mClientNumCompleted)[i],
          numSecs / 60, numSecs % 60,
          (float)PRIV(mClientNumCompleted)[i] / numSecs);
    }
    i++;
  }
  clusterServerPrintProgress(PRIV(mNumCompleted), PRIV(mWidth));
  printf("\n");
  printf("Global speed: %.1f col/sec\n",
      (float)PRIV(mNumCompleted) / difftime(time(NULL), PRIV(mStart)));
  printf("\nEvents:\n");
}

static void		clusterServerHandleClientColumnData(int clientSock)
{
  ClusterServerClass		*self;
  int				column;
  static time_t			lastDisp = 0;

  self = SERVER(ClusterGetSingletonPtr());
  column = PRIV(mClientColumn)[clientSock];
  CLUSTER(self)->protected.mCurrentState\
    ->setColumnFromBuf(CLUSTER(self)->protected.mCurrentState,
        PRIV(mBuf), column);
  PRIV(mColumnsState)[column] = eDone;
  PRIV(mClientNumCompleted)[clientSock]++;
  PRIV(mNumCompleted)++;
  if (difftime(time(NULL), lastDisp) >= 1)
  {
    GraphicsManagerGetSingletonPtr()\
      ->display(IMAGEBUF(CLUSTER(self)->protected.mCurrentState));
    lastDisp = time(NULL);
  }
  self->giveWorkToClient(clientSock);
}

static void		clusterServerRemoveClient(int clientSock)
{
  ClusterServerClass		*self;
  int				column;

  self = SERVER(ClusterGetSingletonPtr());
  column = PRIV(mClientColumn)[clientSock];
  printf("Client %d disconnected [column %d]\n", clientSock, column);
  PRIV(mColumnsState)[column] = eIdle;
  PRIV(mClientColumn)[clientSock] = -1;
  close(clientSock);
  FD_CLR(clientSock, &PRIV(mMasterSet));
}

static void			clusterServerEnd()
{
  ClusterServerClass		*self;
  int				i;

  i = 0;
  self = SERVER(ClusterGetSingletonPtr());
  printf("Rendering finished.\n");
  GraphicsManagerGetSingletonPtr()->end();
  exit(0);
}

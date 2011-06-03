#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "RTEngine.h"
#include "OptionsManager.h"
#include "SceneSerializer.h"
#include "Cluster.h"
#include "ClusterServer.h"
#include "ClusterClient.h"

#ifdef HAS_SDL
#include <SDL/SDL.h>
#endif

static void			launchNormal(char *xmlFile)
{
  SceneSerializerClass		*serializer;

  serializer = new(SceneSerializer, xmlFile);
  serializer->unserialize(serializer);
  RTEngineGetSingletonPtr()->renderScene();
}

static void			launchCluster(char *xmlFile, char *opt)
{
  ClusterClass			*cluster;
  SceneSerializerClass		*serializer;

  if (strcasecmp(opt, "Server") == 0)
  {
    serializer = new(SceneSerializer, xmlFile);
    serializer->unserialize(serializer);
    cluster = new(ClusterServer,
        0, xmlFile);
  }
  else
    cluster = new(ClusterClient, opt);
  cluster->run();
}

int				main(int ac, char **av)
{
  RTEngineClass			*engine;
  OptionsManagerClass		*optionsManager;
  char				*clusterOpt;

  if (ac < 2)
  {
    fprintf(stderr, "Usage: %s [scene.xml|Cluster:[Server|hostip]]\n", av[0]);
    return (1);
  }
  optionsManager = new(OptionsManager, ac, av);
  engine = new(RTEngine);
  clusterOpt = OptionsManagerGetSingletonPtr()\
               ->getOptionValue("Cluster");
  if (clusterOpt == 0)
    launchNormal(av[1]);
  else
    launchCluster(av[1], clusterOpt);
  return (0);
}

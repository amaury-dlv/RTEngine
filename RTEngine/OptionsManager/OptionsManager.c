#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

#include "OptionsManager.h"

static void     optionsManagerConstructor();
static void     optionsManagerDestructor();
static char	*optionsManagerGetOptionValue(char *key);
static char	*optionsManagerGetArg(int n);

/*
 ** The static binding of the functions from the NormeExporter cannot
 ** be done at compile time : "initializer element is not constant".
 ** It has to be done in the contructor.
 */
static OptionsManagerClass       _descr = { /* OptionsManagerClass */
    { /* Class */
      sizeof(OptionsManagerClass),
      0, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
      &optionsManagerConstructor,
      &optionsManagerDestructor,
    },
    &optionsManagerGetOptionValue,
    &optionsManagerGetArg,
    {
      0, /* mNumOptions */
      0 /* mOptionsTab */
    }
  };

Class* OptionsManager = (Class*) &_descr;

static OptionsManagerClass* sSingleton = 0;

static void     optionsManagerConstructor(OptionsManagerClass* self,
					  va_list* list)
{
  if (sSingleton != 0)
    raise("Cannot new a singleton twice : OptionsManager\n");
  sSingleton = self;
  self->private.mNumOptions = va_arg(*list, int);
  self->private.mOptionsTab = va_arg(*list, char**);
}

static void     optionsManagerDestructor(OptionsManagerClass *self)
{
}

OptionsManagerClass*	OptionsManagerGetSingletonPtr(void)
{
  return (sSingleton);
}

static char		*optionsManagerGetOptionValue(char *key)
{
  OptionsManagerClass	*this;
  int			i;
  char			*res;

  i = 0;
  this = OptionsManagerGetSingletonPtr();
  while (i < this->private.mNumOptions)
  {
    if (strncasecmp(this->private.mOptionsTab[i], key, strlen(key)) == 0
        && this->private.mOptionsTab[i][strlen(key)] == ':')
    {
      res = this->private.mOptionsTab[i] + strlen(key);
      if (*res == 0 || *(res + 1) == 0)
        return (0);
      return (res + 1);
    }
    i++;
  }
  return (0);
}

static char		*optionsManagerGetArg(int n)
{
  OptionsManagerClass	*this;

  this  = OptionsManagerGetSingletonPtr();
  if (n >= this->private.mNumOptions)
    raise("No such arg.\n");
  return (this->private.mOptionsTab[n]);
}

#ifndef __OPTIONSMANAGER_H__
# define __OPTIONSMANAGER_H__

# include "OOC.h"

typedef struct OptionsManagerClass	OptionsManagerClass;
struct                          	OptionsManagerClass
{
  /* SuperClass : */
  Class				__base__;
  /* public : */
  char				*(*getOptionValue)(char *key);
  char				*(*getArg)(int n);
  /* private : */
  struct
  {
    int				mNumOptions;
    char			**mOptionsTab;
  } private;
};

OptionsManagerClass* 		OptionsManagerGetSingletonPtr(void);

extern Class* 			OptionsManager;

#endif /* !__OPTIONSMANAGER_H__ */

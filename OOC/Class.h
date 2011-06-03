#ifndef		__CLASS_H__
# define	__CLASS_H__

# include <sys/types.h>
# include <stdarg.h>

# include "bool.h"
# include "raise.h"

# define VOID(x)	((void*) (x))
# define CLASS(x)	((Class*) (x))
# define PRIV(x)	(self->private.x)
# define PROT(x)	(self->protected.x)

# define RTTI(x)	(CLASS(x)->__RTTI__)

typedef void Object;

typedef struct	       	Class
{
  const size_t		__size__;
  int			__RTTI__;
  void			(*__constructor__)();
  void			(*__destructor__)();
}			Class;

#endif		/* !__CLASS_H__ */

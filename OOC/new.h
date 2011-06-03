#ifndef		__NEW_H__
# define	__NEW_H__

# include <stdarg.h>
# include "Class.h"

Object*		new(Class *class, ...);
Object*		va_new(Class *class, va_list *ap);
void		delete(Object  *ptr);

#endif		/* !__NEW_H__ */

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "raise.h"
#include "Class.h"
#include "new.h"

Object*		new(Class *class, ...)
{
  va_list	ap;
  Object*	ret;

  va_start(ap, class);
  ret = va_new(class, &ap);
  va_end(ap);
  return (ret);
}

Object*		va_new(Class *class, va_list *ap)
{
  Object*	ret;

  if (!(ret = malloc(class->__size__)))
    raise("Out of memory\n");
  memcpy(ret, class, class->__size__);
  if (class->__constructor__)
    class->__constructor__(ret, ap);
  return (ret);
}

void		delete(Object *ptr)
{
  Class*	class;

  if (ptr)
    {
      class = (Class *)ptr;
      if (class->__destructor__)
	class->__destructor__(ptr);
      free(ptr);
    }
}

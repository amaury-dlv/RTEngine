#ifndef __XMLDOCUMENT_H__
# define __XMLDOCUMENT_H__

# include <libxml/xmlmemory.h>
# include <libxml/parser.h>

# include "OOC.h"

# define XMLSTR(x)		((xmlChar*) (x))

typedef struct XMLDocumentClass	XMLDocumentClass;
struct				XMLDocumentClass
{
  /* SuperClass : */
  Class				__base__;
  /* public : */
  int				(*init)(XMLDocumentClass *self);
  xmlNodePtr			(*getRoot)(XMLDocumentClass *self);
  xmlNodePtr			(*getNodeByName)(XMLDocumentClass *self,
                                                 xmlNodePtr node,
                                                 char *name);
  void				(*vSetStrsFromProp)(XMLDocumentClass *self,
                                                    xmlNodePtr, va_list *list);
  void				(*putError)(XMLDocumentClass *self,
                                            xmlNodePtr node,
                                            char *format, ...);
  void				(*vPutError)(XMLDocumentClass *self,
                                             xmlNodePtr node,
                                             char *format, va_list *list);
  /* private : */
  struct
  {
    char			*mPath;
    xmlDocPtr			mDoc;
    xmlNodePtr			mRoot;
  } private;
};

extern Class			*XMLDocument;

#endif /* !__XMLDOCUMENT_H__ */

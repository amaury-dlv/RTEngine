#include <string.h>

#include "XMLDocument.h"

static void     XMLDocumentConstructor(XMLDocumentClass *self, va_list *list);
static void     XMLDocumentDestructor(XMLDocumentClass *self);
static xmlNodePtr	XMLDocumentGetRoot(XMLDocumentClass *self);
static int	XMLDocumentInit(XMLDocumentClass *self);
static xmlNodePtr	XMLDocumentGetNodeByName(XMLDocumentClass *self,
                                                 xmlNodePtr node,
                                                 char *name);
static void	XMLDocumentVPutError(XMLDocumentClass *self,
                                    xmlNodePtr node, char *msg, va_list *list);
static void	XMLDocumentPutError(XMLDocumentClass *self,
                                    xmlNodePtr node,
                                    char *format,
                                            ...);
static void	XMLDocumentVSetStrsFromProp(XMLDocumentClass *self,
                                            xmlNodePtr node,
                                            va_list *list);

static XMLDocumentClass       _descr = { /* XMLDocumentClass */
  { /* Class */
    sizeof(XMLDocumentClass),
    0, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
    &XMLDocumentConstructor,
    &XMLDocumentDestructor,
  },
  &XMLDocumentInit,
  &XMLDocumentGetRoot,
  &XMLDocumentGetNodeByName,
  &XMLDocumentVSetStrsFromProp,
  &XMLDocumentPutError,
  &XMLDocumentVPutError,
  { /* private */
    0, /* mPath */
    0, /* mDoc */
    0 /* mRoot */
  }
};

Class* XMLDocument = (Class*) &_descr;

static void     XMLDocumentConstructor(XMLDocumentClass *self, va_list *list)
{
  self->private.mPath = strdup(va_arg(*list, char*));
  xmlLineNumbersDefault(1);
}

static void     XMLDocumentDestructor(XMLDocumentClass *self)
{
  xmlFreeDoc(self->private.mDoc);
}

static xmlNodePtr	XMLDocumentGetRoot(XMLDocumentClass *self)
{
  return (self->private.mRoot);
}

static int	XMLDocumentInit(XMLDocumentClass *self)
{
  if (!(self->private.mDoc = xmlParseFile(self->private.mPath)))
  {
    fprintf(stderr, "Document not parsed successfully.\n");
    exit(1);
  }
  if (!(self->private.mRoot = xmlDocGetRootElement(self->private.mDoc)))
  {
    fprintf(stderr, "Empty document.\n");
    exit(1);
  }
  return (0);
}

static void		XMLDocumentVSetStrsFromProp(XMLDocumentClass *self,
                                                    xmlNodePtr node,
                                                    va_list *list)
{
  char			**s;
  char			*prop;
  char			*content;

  while ((prop = va_arg(*list, char*)))
  {
    s = va_arg(*list, char**);
    content = (char*) xmlGetProp(node, XMLSTR(prop));
    if (content == 0)
      self->putError(self, node, "Error: missing attribute : %s\n", prop);
    *s = content;
  }
}

static xmlNodePtr	XMLDocumentGetNodeByName(XMLDocumentClass *self,
                                                 xmlNodePtr node,
                                                 char *name)
{
  while (node != 0)
  {
    if (xmlStrcmp(node->name, XMLSTR(name)) == 0)
      return (node);
    node = node->next;
  }
  return (node);
}

static void		XMLDocumentPutError(XMLDocumentClass *self,
                                            xmlNodePtr node,
                                            char *format,
                                            ...)
{
  va_list	list;

  va_start(list, format);
  self->vPutError(self, node, format, &list);
  va_end(list);
}

static void		XMLDocumentVPutError(XMLDocumentClass *self,
                                    xmlNodePtr node,
                                    char *format,
                                    va_list *list)
{
  int			line;
  xmlChar		*pathStr;
  xmlBufferPtr		nodeStr;

  pathStr = NULL;
  nodeStr = xmlBufferCreate();
  pathStr = xmlGetNodePath(node->parent);
  xmlNodeDump(nodeStr, self->private.mDoc, node, 0, 1);
  line = xmlGetLineNo(node);
  fprintf(stderr, "%s:%d: %s\n", self->private.mPath, line, (char*) pathStr);
  fprintf(stderr, "%s\n", nodeStr->content); // tmp
  vfprintf(stderr, format, *list);
  xmlFree(pathStr);
  xmlFree(nodeStr);
  exit(1);
}

#include <stdio.h>

#include "Mesh.h"
#include "FlatMeshTriangle.h"
#include "SmoothMeshTriangle.h"
#include "RenderingScene.h"
#include "SceneSerializer.h"
#include "Utils.h"
#include "Maths.h"

static void     meshConstructor(MeshClass *self, va_list *list);
static void     meshDestructor();
static void	meshComputeMeshNormals(MeshClass *self);
static void	meshReadRescaleVertices(MeshClass *self,
                                        ReadStruct *s);
static void	meshReadHandleVertexes(MeshClass *self,
                                       ReadStruct *s);
static void	meshReadAddFlat(MeshClass *self,
                                FaceStruct *face,
                                ReadStruct *s);
static void	meshReadAddSmooth(MeshClass *self,
                                  FaceStruct *face,
                                  ReadStruct *s,
                                  int *count);
static void	meshReadAddTriangles(MeshClass *self,
                                     ReadStruct *s);
static void	meshReadHandleFaces(MeshClass *self,
                                    ReadStruct *s);
static int	meshReadPlyLoop(MeshClass *self, ReadStruct *s);
static int	meshReadPly(MeshClass *self, char *name);

static MeshClass       _descr = { /* MeshClass */
  { /* Class */
    sizeof(MeshClass),
    __RTTI_MESH,
    &meshConstructor,
    &meshDestructor,
  },
  0, /* vertices */
  0, /* normala */
  0, /* u */
  0, /* v*/
  0, /* vertexFaces */
  0, /* sizeVertexFaces */
  0, /* numVertices */
  0, /* numTriangles */
  0, /* material */
  eFlat, /* type */
  {
    0, /* mGrid */
    0, /* mMaxCoord */
    0, /* mSize */
  }
};

Class *Mesh = (Class*) &_descr;

PlyProperty vert_props[] = {
  {"x", PLY_FLOAT, PLY_FLOAT, offsetof(VertexStruct, x), 0, 0, 0, 0},
  {"y", PLY_FLOAT, PLY_FLOAT, offsetof(VertexStruct, y), 0, 0, 0, 0},
  {"z", PLY_FLOAT, PLY_FLOAT, offsetof(VertexStruct, z), 0, 0, 0, 0}
};

PlyProperty face_props[] = {
  {"vertex_indices", PLY_INT, PLY_INT, offsetof(FaceStruct, verts),
    1, PLY_UCHAR, PLY_UCHAR, offsetof(FaceStruct, nverts)}
};

static void     meshConstructor(MeshClass *self, va_list *list)
{
  self->vertices = new(Vector);
  self->normals = new(Vector);
  self->private.mGrid = new(Grid);
}

static void     meshDestructor()
{
}

void	*meshUnserialize(SceneSerializerClass *serializer,
                                 xmlNodePtr node)
{
  MeshClass	*self;
  char		*strName, *strIdMat, *strType, *strSize;

  self = new(Mesh);
  serializer->setStrsFromProp(serializer, node, "file", &strName,
      "id_mat", &strIdMat, "shape", &strType, "size", &strSize, NULL);
  serializer->setStrsFromProp(serializer, node, "id_mat", &strIdMat, NULL);
  if (!(self->material = serializer->getMaterialFromId(serializer, strIdMat))
      || (setFloatFromStr(strSize, &self->private.mSize) != 0))
  {
    fprintf(stderr, "Error: bad attributes\n");
    return (0);
  }
  if (strcmp(strType, "flat") == 0) self->type = eFlat;
  else if (strcmp(strType, "smooth") == 0)
    self->type = eSmooth;
  else
  {
    fprintf(stderr, "Error: bad attributes\n");
    return (0);
  }
  meshReadPly(self, strName);
  return (self->private.mGrid);
}

static void		meshComputeMeshNormals(MeshClass *self)
{
  int			j, index;
  GridClass		*grid;
  Vector3D		*normal;

  printf("Mesh: computing smooth mesh normals\n");
  index = 0;
  grid = self->private.mGrid;
  while (index < self->numVertices)
  {
    j = 0;
    normal = xmalloc(sizeof(*normal));
    *normal = (Vector3D){0, 0, 0};
    while (j < self->sizeVertexFaces[index])
      vectOpPlus(normal,
          &TRIANGLE(grid->objects->mPtrs[self->vertexFaces[index][j++]])\
          ->normal);
    if (normal->x == 0. && normal->y == 0. && normal->z == 0.)
      normal->y = 1.;
    else
      vectNormalize(normal);
    self->normals->push_back(self->normals, normal);
    index++;
  }
}

static void	meshReadRescaleVertices(MeshClass *self,
                                        ReadStruct *s)
{
  int		j;

  j = 0;
  while (j < s->num_elems)
  {
    Point3D *vertex;
    vertex = self->vertices->mPtrs[j];
    vertex->x /= self->private.mMaxCoord;
    vertex->y /= self->private.mMaxCoord;
    vertex->z /= self->private.mMaxCoord;
    vertex->x *= self->private.mSize;
    vertex->y *= self->private.mSize;
    vertex->z *= self->private.mSize;
    j++;
  }
}

static void	meshReadHandleVertexes(MeshClass *self,
                                       ReadStruct *s)
{
  int		j;
  VertexStruct	vertex;
  Point3D	*point;

  ply_get_property(s->ply, s->elem_name, &vert_props[0]);
  ply_get_property(s->ply, s->elem_name, &vert_props[1]);
  ply_get_property(s->ply, s->elem_name, &vert_props[2]);
  self->numVertices = s->num_elems;
  j = 0;
  while (j < s->num_elems)
  {
    ply_get_element(s->ply, &vertex);
    point = xmalloc(sizeof(*point));
    *point = (Point3D){vertex.x, vertex.y, vertex.z};
    self->vertices->push_back(self->vertices, point);
    self->private.mMaxCoord = MAX(self->private.mMaxCoord, ABS(vertex.x));
    self->private.mMaxCoord = MAX(self->private.mMaxCoord, ABS(vertex.y));
    self->private.mMaxCoord = MAX(self->private.mMaxCoord, ABS(vertex.z));
    j++;
  }
  printf("Mesh: %d vertices\n", j);
  meshReadRescaleVertices(self, s);
}

static void	meshReadAddFlat(MeshClass *self,
                                FaceStruct *face,
                                ReadStruct *s)
{
  GridClass		*grid;
  MeshTriangleClass	*triangle;

  grid = self->private.mGrid;
  triangle = new(FlatMeshTriangle, self->material, self,
      face->verts[0], face->verts[1], face->verts[2]);
  triangle->computeNormal(triangle, 1);
  grid->addObject(grid, GEOOBJECT(triangle));
}

static void	meshReadAddSmooth(MeshClass *self,
                                  FaceStruct *face,
                                  ReadStruct *s,
                                  int *count)
{
  GridClass		*grid;
  MeshTriangleClass	*triangle;

  grid = self->private.mGrid;
  triangle = new(SmoothMeshTriangle, self->material, self,
      face->verts[0], face->verts[1], face->verts[2]);
  grid->addObject(grid, GEOOBJECT(triangle));
  triangle->computeNormal(triangle, 0);
  self->vertexFaces[face->verts[0]][self->sizeVertexFaces[face->verts[0]]++]
    = *count;
  self->vertexFaces[face->verts[1]][self->sizeVertexFaces[face->verts[1]]++]
    = *count;
  self->vertexFaces[face->verts[2]][self->sizeVertexFaces[face->verts[2]]++]
    = *count;
  (*count)++;
}

static void	meshReadAddTriangles(MeshClass *self,
                                     ReadStruct *s)
{
  int		j;
  FaceStruct	face;
  int		count;

  j = 0;
  count = 0;
  printf("Mesh: %d triangles\n", s->num_elems);
  while (j < s->num_elems)
  {
    ply_get_element(s->ply, &face);
    if (self->type == eFlat)
      meshReadAddFlat(self, &face, s);
    else
      meshReadAddSmooth(self, &face, s, &count);
    j++;
  }
}

static void	meshReadHandleFaces(MeshClass *self,
                                    ReadStruct *s)
{
  int		j;

  ply_get_property(s->ply, s->elem_name, &face_props[0]);
  self->numTriangles = s->num_elems;
  self->vertexFaces =
    xmalloc(sizeof(*self->vertexFaces) * self->numVertices);
  self->sizeVertexFaces =
    xmalloc(sizeof(*self->sizeVertexFaces) * self->numVertices);
  j = 0;
  while (j < self->numVertices)
  {
    self->vertexFaces[j] =
      xmalloc(sizeof(**self->vertexFaces) * 100);
    self->sizeVertexFaces[j] = 0;
    j++;
  }
  meshReadAddTriangles(self, s);
}

static int	meshReadPlyLoop(MeshClass *self, ReadStruct *s)
{
  int		i;

  i = 0;
  while (i < s->nelems)
  {
    s->elem_name = s->elist[i];
    s->plist = ply_get_element_description(s->ply,
                                           s->elem_name,
                                           &s->num_elems,
                                           &s->nprops);
    if (strcmp("vertex", s->elem_name) == 0)
      meshReadHandleVertexes(self, s);
    if (strcmp("face", s->elem_name) == 0)
      meshReadHandleFaces(self, s);
    i++;
  }
  return (0);
}

static int	meshReadPly(MeshClass *self, char *name)
{
  int		i;
  ReadStruct	s;

  i = 0;
  printf("Mesh: opening mesh file: %s\n", name);
  if (!(s.ply = ply_open_for_reading(name, &s.nelems,
          &s.elist, &s.file_type, &s.version)))
  {
    printf("Unable to read file\n");
    exit(1);
  }
  meshReadPlyLoop(self, &s);
  ply_close(s.ply);
  if (self->type == eSmooth)
    meshComputeMeshNormals(self);
  printf("Mesh: putting triangles in a grid:\n");
  self->private.mGrid->setupCells(self->private.mGrid);
  GEOOBJECT(self->private.mGrid)->material = 0;
  printf("Mesh: done\n");
  return (0);
}

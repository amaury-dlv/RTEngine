#ifndef __MESH_H__
# define __MESH_H__

# include "OOC.h"
# include "Vector.h"
# include "ply.h"
# include "Material.h"
# include "Scene.h"
# include "Grid.h"

typedef enum MeshType		MeshType;
enum				MeshType
{
  eFlat,
  eSmooth
};

typedef struct VertexStruct	VertexStruct;
struct				VertexStruct
{
  float			x;
  float			y;
  float			z;
};

typedef struct FaceStruct	FaceStruct;
struct				FaceStruct
{
  unsigned char		nverts;
  int			*verts;
};

typedef struct ReadStruct	ReadStruct;
struct				ReadStruct
{
  int			i;
  int			j;
  PlyFile		*ply;
  int			nelems;
  char			**elist;
  int			file_type;
  float			version;
  int			nprops;
  int			num_elems;
  PlyProperty		**plist;
  VertexStruct		**vlist;
  FaceStruct		**flist;
  char			*elem_name;
  int			num_comments;
  char			**comments;
  int			num_obj_info;
  char			**obj_info;
};

typedef struct MeshClass	MeshClass;
struct                          MeshClass
{
  /* SuperClass : */
  Class				__base__;
  /* public : */
  VectorClass			*vertices;
  VectorClass			*normals;
  VectorClass			*u;
  VectorClass			*v;
  int				**vertexFaces;
  int				*sizeVertexFaces;
  int				numVertices;
  int				numTriangles;
  MaterialClass			*material;
  MeshType			type;
  struct
  {
    GridClass			*mGrid;
    double			mMaxCoord;
    float			mSize;
  } private;
};

extern Class*			Mesh;

extern PlyProperty		vert_props[3];
extern PlyProperty		face_props[1];

void				*meshUnserialize();

#endif /* !__MESH_H__ */

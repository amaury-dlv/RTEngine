#ifndef __GRID_H__
# define __GRID_H__

# include "GeometricObject.h"
# include "Vector3D.h"
# include "Point3D.h"
# include "BBox.h"
# include "HitRecord.h"

# define gridKEpsilon 1.1

typedef struct Poubelle		Poubelle;
struct				Poubelle
{
  double			ox;
  double			oy;
  double			oz;
  double			dx;
  double			dy;
  double			dz;
  double			x0;
  double			y0;
  double			z0;
  double			x1;
  double			y1;
  double			z1;
  double			tx_min;
  double			ty_min;
  double			tz_min;
  double			tx_max;
  double			ty_max;
  double			tz_max;
  double			a;
  double			b;
  double			c;
  double			t0;
  double			t1;
  int				ix;
  int				iy;
  int				iz;
  double			dtx;
  double			dty;
  double			dtz;
  double			tx_next;
  double			ty_next;
  double			tz_next;
  int				ix_step;
  int				iy_step;
  int				iz_step;
  int				ix_stop;
  int				iy_stop;
  int				iz_stop;
  Point3D			pp;
  VectorClass			*cellVect;
  int				ret;
  RayStruct			*ray;
  double			*tMin;
  HitRecordStruct		*rec;
};

typedef struct GridClass	GridClass;
struct                          GridClass
{
  /* SuperClass : */
  GeometricObjectClass		__base__;
  /* public : */
  void				(*addObject)(GridClass *self,
                                             GeometricObjectClass *object);
  void				(*setupCells)(GridClass *self);
  VectorClass			*objects;
  int				numCells;
  VectorClass			**cells;
  int				nx;
  int				ny;
  int				nz;
  BBox				BBox;
  /* private : */
  struct
  {
    /* lastRay */
  } private;
};

extern Class*			Grid;

#endif /* !__GRID_H__ */

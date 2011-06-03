#include <stdio.h>
#include <math.h>

#include "Grid.h"
#include "Ray.h"
#include "HitRecord.h"
#include "Maths.h"
#include "Material.h"
#include "Utils.h"
#include "MeshTriangle.h"

static void     gridConstructor(GridClass *self, va_list *list);
static void     gridDestructor(GridClass *self);
static void	gridAddObject(GridClass *self, GeometricObjectClass *object);
static Point3D	gridMinBounds(GridClass *self);
static Point3D	gridMaxBounds(GridClass *self);
static void	gridInitSetup(GridClass *self, Point3D *p0, Point3D *p1);
static void	gridSetLim(GridClass *self, BBox *objBBox,
                           Point3D *p0, Point3D *p1, int *lim);
static void	gridSetupLoop(GridClass *self,
                              GeometricObjectClass *obj, int *lim);
static void	gridSetupCells(GridClass *self);
static bool	gridCellHit(GridClass *self,
                            RayStruct *ray,
                            double *tMin,
                            HitRecordStruct *rec,
                            VectorClass *cellVect);
static bool	gridHit(GridClass *self, RayStruct *ray, double *tMin);
static void	gridHitInit(GridClass *self,
                            Poubelle *p,
                            RayStruct *ray,
                            double *t,
                            HitRecordStruct *rec);
static void	gridHita(GridClass *self, Poubelle *p);
static void	gridHitb(GridClass *self, Poubelle *p);
static bool	gridHitCheck(GridClass *self, Poubelle *p);
static void	gridHitInitStart(GridClass *self, Poubelle *p);
static void	gridInita(GridClass *self, Poubelle *p);
static void	gridInitb(GridClass *self, Poubelle *p);
static void	gridInitc(GridClass *self, Poubelle *p);
static void	gridInitAll(GridClass *self, Poubelle *p);
static int	gridHit1(GridClass *self, Poubelle *p);
static int	gridHit2(GridClass *self, Poubelle *p);
static bool	gridHitWithRecord(GridClass *self, RayStruct *ray,
                                  double *tMin, HitRecordStruct *rec);
/*
** The static binding of the functions from the NormeExporter cannot
** be done at compile time : "initializer element is not constant".
** It has to be done in the contructor.
*/
static GridClass       _descr = { /* GridClass */
  { /* GeometricObjectClass */
    { /* Class */
      sizeof(GridClass),
      __RTTI_GRID, /* RTTI (not used for as long as we can... dynamic binding does have a cost) */
      &gridConstructor,
      &gridDestructor,
    },
    0, /* GeometricObjectNE1.getId will be set in the constructor */
    &gridHit, /* hit() pseudo virtual function (the binding is static not dynamic!) */
    &gridHitWithRecord, /* hitWithRecord() */
    0, /* hit() = 0; Pure virtual */
    0, /* hitWithRecord() = 0; Pure virtual */
    0, /* getNormal() ; virtual */
    0, /* pdf() ; virtual */
    0, /* sample() */
    0, /* mId */
    0, /* material */
    {0, 0, 0}, /* mPos */
    {
      0, /* mRot */
      0, /* mInvArea */
      0 /* mSampler */
    }
  },
  &gridAddObject,
  &gridSetupCells,
  0, /* objects */
  0, /* numCells */
  0, /* cells */
  0, /* nx */
  0, /* ny */
  0, /* nz */
  {0, 0, 0, 0, 0, 0}, /* BBox */
  {
  }
};

Class *Grid = (Class*) &_descr;

static void     gridConstructor(GridClass *self, va_list *list)
{
  GeometricObject->__constructor__(self, list);
  self->objects = new(Vector);
}

static void     gridDestructor(GridClass *self)
{
  GeometricObject->__destructor__(self);
}

static void	gridAddObject(GridClass *self, GeometricObjectClass *object)
{
  self->objects->push_back(self->objects, object);
}

static Point3D	gridMinBounds(GridClass *self)
{
  BBox		objBox;
  GeometricObjectClass	*obj;
  Point3D		bound = {Infinity, Infinity, Infinity};
  int			i;

  i = 0;
  while (i < self->objects->size(self->objects))
  {
    obj = self->objects->mPtrs[i];
    objBox = meshTriangleGetBBox(TRIANGLE(obj));
    bound.x = MIN(bound.x, objBox.x0);
    bound.y = MIN(bound.y, objBox.y0);
    bound.z = MIN(bound.z, objBox.z0);
    i++;
  }
  bound.x -= gridKEpsilon;
  bound.y -= gridKEpsilon;
  bound.z -= gridKEpsilon;
  return (bound);
}

static Point3D	gridMaxBounds(GridClass *self)
{
  BBox		objBox;
  GeometricObjectClass	*obj;
  Point3D		bound = {-Infinity, -Infinity, -Infinity};
  int			i;

  i = 0;
  while (i < self->objects->size(self->objects))
  {
    obj = self->objects->mPtrs[i];
    objBox = meshTriangleGetBBox(TRIANGLE(obj));
    bound.x = MAX(bound.x, objBox.x1);
    bound.y = MAX(bound.y, objBox.y1);
    bound.z = MAX(bound.z, objBox.z1);
    i++;
  }
  bound.x += gridKEpsilon;
  bound.y += gridKEpsilon;
  bound.z += gridKEpsilon;
  return (bound);
}

static void	gridInitSetup(GridClass *self, Point3D *p0, Point3D *p1)
{
  self->BBox.x0 = p0->x, self->BBox.y0 = p0->y, self->BBox.z0 = p0->z;
  self->BBox.x1 = p1->x, self->BBox.y1 = p1->y, self->BBox.z1 = p1->z;
  int numObjects = self->objects->size(self->objects);
  printf(" Grid: setting up regular grid...\n");
  printf(" Grid: grid bounding box (%.2f, %.2f, %.2f) | (%.2f, %.2f, %.2f)\n",
      self->BBox.x0, self->BBox.y0, self->BBox.z0,
      self->BBox.x1, self->BBox.y1, self->BBox.z1);
  printf(" Grid: %d objects\n", numObjects);
  double wx = p1->x - p0->x;
  double wy = p1->y - p0->y;
  double wz = p1->z - p0->z;
  self->nx = 2.0 * (wx)
    / pow(wx * wy * wz / numObjects, 0.333333) + 1;
  self->ny = 2.0 * (wy)
    / pow(wx * wy * wz / numObjects, 0.333333) + 1;
  self->nz = 2.0 * (wz)
    / pow(wx * wy * wz / numObjects, 0.333333) + 1;
  self->numCells = self->nx * self->ny * self->nz;
  printf(" Grid: size x:%d y:%d z:%d\n", self->nx, self->ny, self->nz);
  printf(" Grid: %d cells\n", self->numCells);
  int i = 0;
  self->cells = xmalloc(sizeof(*self->cells) * self->numCells);
  while (i < self->numCells)
    self->cells[i++] = new(Vector);
}

static void	gridSetLim(GridClass *self, BBox *objBBox,
    Point3D *p0, Point3D *p1, int *lim)
{
  lim[0] = clamp((objBBox->x0 - p0->x) * ((double) self->nx)
      / (p1->x - p0->x), 0, ((double)self->nx) - 1);
  lim[10] = clamp((objBBox->y0 - p0->y) * ((double) self->ny)
      / (p1->y - p0->y), 0, ((double)self->ny) - 1);
  lim[20] = clamp((objBBox->z0 - p0->z) * ((double) self->nz)
      / (p1->z - p0->z), 0, ((double)self->nz) - 1);
  lim[01] = clamp(ceil((objBBox->x1 - p0->x) * ((double) self->nx)
        / (p1->x - p0->x)), 0, ((double)self->nx) - 1);
  lim[11] = clamp(ceil((objBBox->y1 - p0->y) * ((double) self->ny)
        / (p1->y - p0->y)), 0, ((double)self->ny) - 1);
  lim[21] = clamp(ceil((objBBox->z1 - p0->z) * ((double) self->nz)
        / (p1->z - p0->z)), 0, ((double)self->nz) - 1);
}

static void	gridSetupLoop(GridClass *self,
                              GeometricObjectClass *obj,
                              int *lim)
{
  int		ix;
  int		iy;
  int		iz;
  int		index;

  iz = lim[20];
  while (iz < lim[21])
  {
    iy = lim[10];
    while (iy < lim[11])
    {
      ix = lim[00];
      while (ix < lim[01])
      {
        index = ix + self->nx * iy + self->nx * self->ny * iz;
        self->cells[index]->push_back(self->cells[index], obj);
        ix++;
      }
      iy++;
    }
    iz++;
  }
}

static void	gridSetupCells(GridClass *self)
{
  GeometricObjectClass *obj;
  BBox			objBBox;
  int			lim[33];
  int			i;
  Point3D		p0 = gridMinBounds(self);
  Point3D		p1 = gridMaxBounds(self);

  i = 0;
  gridInitSetup(self, &p0, &p1);
  while (i < self->objects->size(self->objects))
  {
    obj = self->objects->mPtrs[i];
    objBBox = meshTriangleGetBBox(TRIANGLE(obj));
    gridSetLim(self, &objBBox, &p0, &p1, lim);
    gridSetupLoop(self, obj, lim);
    i++;
  }
  printf(" Grid: grid created\n");
}

static bool		gridCellHit(GridClass *self,
                                    RayStruct *ray,
                                    double *tMin,
                                    HitRecordStruct *rec,
                                    VectorClass *cellVect)
{
  int			i;
  double		t;
  GeometricObjectClass	*obj;
  GeometricObjectClass	*closestObj = NULL;

  i = 0;
  t = Infinity;
  while (i < cellVect->size(cellVect))
  {
    obj = cellVect->mPtrs[i];
    if (obj->objHit(obj, ray, &t) && t < *tMin)
    {
      *tMin = t;
      closestObj = obj;
    }
    i++;
  }
  if (closestObj)
  {
    closestObj->objHitWithRecord(closestObj, ray, &t, rec);
    return (true);
  }
  return (false);
}

static bool	gridHit(GridClass *self, RayStruct *ray, double *tMin)
{
  HitRecordStruct 	rec;
  bool			hit;

  hit = gridHitWithRecord(self, ray, tMin, &rec);
  return (hit);
}

static void	gridHitInit(GridClass *self,
                            Poubelle *p,
                            RayStruct *ray,
                            double *t,
                            HitRecordStruct *rec)
{
  p->ray = ray;
  p->tMin = t;
  p->rec = rec;
  p->ox = ray->origin.x;
  p->oy = ray->origin.y;
  p->oz = ray->origin.z;
  p->dx = ray->direction.x;
  p->dy = ray->direction.y;
  p->dz = ray->direction.z;
  p->x0 = self->BBox.x0;
  p->y0 = self->BBox.y0;
  p->z0 = self->BBox.z0;
  p->x1 = self->BBox.x1;
  p->y1 = self->BBox.y1;
  p->z1 = self->BBox.z1;
}

static void	gridHita(GridClass *self, Poubelle *p)
{
  p->a = 1.0 / p->dx;
  if (p->a >= 0)
  {
    p->tx_min = (p->x0 - p->ox) * p->a;
    p->tx_max = (p->x1 - p->ox) * p->a;
  }
  else
  {
    p->tx_min = (p->x1 - p->ox) * p->a;
    p->tx_max = (p->x0 - p->ox) * p->a;
  }
  p->b = 1.0 / p->dy;
  if (p->b >= 0)
  {
    p->ty_min = (p->y0 - p->oy) * p->b;
    p->ty_max = (p->y1 - p->oy) * p->b;
  }
  else
  {
    p->ty_min = (p->y1 - p->oy) * p->b;
    p->ty_max = (p->y0 - p->oy) * p->b;
  }
}

static void	gridHitb(GridClass *self, Poubelle *p)
{
  p->c = 1.0 / p->dz;
  if (p->c >= 0)
  {
    p->tz_min = (p->z0 - p->oz) * p->c;
    p->tz_max = (p->z1 - p->oz) * p->c;
  }
  else
  {
    p->tz_min = (p->z1 - p->oz) * p->c;
    p->tz_max = (p->z0 - p->oz) * p->c;
  }
}

static bool	gridHitCheck(GridClass *self, Poubelle *p)
{
  if (p->tx_min > p->ty_min)
    p->t0 = p->tx_min;
  else
    p->t0 = p->ty_min;
  if (p->tz_min > p->t0)
    p->t0 = p->tz_min;
  if (p->tx_max < p->ty_max)
    p->t1 = p->tx_max;
  else
    p->t1 = p->ty_max;
  if (p->tz_max < p->t1)
    p->t1 = p->tz_max;
  if (p->t0 > p->t1)
    return (false);
  return (true);
}

static void	gridHitInitStart(GridClass *self, Poubelle *p)
{
  if (inside(&self->BBox, &p->ray->origin))
  {
    p->ix = clamp((p->ox - p->x0) * ((double)self->nx) / (p->x1 - p->x0), 0,
        ((double)self->nx) - 1);
    p->iy = clamp((p->oy - p->y0) * ((double)self->ny) / (p->y1 - p->y0), 0,
        ((double)self->ny) - 1);
    p->iz = clamp((p->oz - p->z0) * ((double)self->nz) / (p->z1 - p->z0), 0,
        ((double)self->nz) - 1);
  }
  else
  {
    p->pp = p->ray->direction;
    vectOpTimes(&p->pp, p->t0);
    vectOpPlus(&p->pp, &p->ray->origin);
    p->ix = clamp((p->pp.x - p->x0) * ((double)self->nx) / (p->x1 - p->x0),
        0, ((double)self->nx) - 1);
    p->iy = clamp((p->pp.y - p->y0) * ((double)self->ny) / (p->y1 - p->y0),
        0, ((double)self->ny) - 1);
    p->iz = clamp((p->pp.z - p->z0) * ((double)self->nz) / (p->z1 - p->z0),
        0, ((double)self->nz) - 1);
  }
}

static void	gridInita(GridClass *self, Poubelle *p)
{
  if (p->dx > 0)
  {
    p->tx_next = p->tx_min + (p->ix + 1) * p->dtx;
    p->ix_step = 1;
    p->ix_stop = ((double)self->nx);
  }
  else
  {
    p->tx_next = p->tx_min + (((double)self->nx) - p->ix) * p->dtx;
    p->ix_step = -1;
    p->ix_stop = -1;
  }
  if (p->dx == 0.0)
  {
    p->tx_next = Infinity;
    p->ix_step = -1;
    p->ix_stop = -1;
  }
}

static void	gridInitb(GridClass *self, Poubelle *p)
{
  if (p->dy > 0)
  {
    p->ty_next = p->ty_min + (p->iy + 1) * p->dty;
    p->iy_step = 1;
    p->iy_stop = ((double)self->ny);
  }
  else
  {
    p->ty_next = p->ty_min + (((double)self->ny) - p->iy) * p->dty;
    p->iy_step = -1;
    p->iy_stop = -1;
  }
  if (p->dy == 0.0)
  {
    p->ty_next = Infinity;
    p->iy_step = -1;
    p->iy_stop = -1;
  }
}

static void	gridInitc(GridClass *self, Poubelle *p)
{
  if (p->dz > 0.0)
  {
    p->tz_next = p->tz_min + (p->iz + 1) * p->dtz;
    p->iz_step = 1;
    p->iz_stop = ((double)self->nz);
  }
  else
  {
    p->tz_next = p->tz_min + (((double)self->nz) - p->iz) * p->dtz;
    p->iz_step = -1;
    p->iz_stop = -1;
  }
  if (p->dz == 0.0)
  {
    p->tz_next = Infinity;
    p->iz_step = -1;
    p->iz_stop = -1;
  }
}

static void	gridInitAll(GridClass *self, Poubelle *p)
{
  gridInita(self, p);
  gridInitb(self, p);
  gridInitc(self, p);
}

static int	gridHit1(GridClass *self, Poubelle *p)
{
  if (gridCellHit(self, p->ray, p->tMin, p->rec, p->cellVect))// && *tMin < tx_next)
    return (true);
  p->tx_next += p->dtx;
  p->ix += p->ix_step;
  if (p->ix == p->ix_stop)
    return (false);
  return (-1);
}

static int	gridHit2(GridClass *self, Poubelle *p)
{
  if (p->ty_next < p->tz_next)
  {
    if (gridCellHit(self, p->ray, p->tMin, p->rec, p->cellVect))// && *tMin < ty_next)
      return (true);
    p->ty_next += p->dty;
    p->iy += p->iy_step;
    if (p->iy == p->iy_stop)
      return (false);
  }
  else
  {
    if (gridCellHit(self, p->ray, p->tMin, p->rec, p->cellVect))// && *tMin < tz_next)
      return (true);
    p->tz_next += p->dtz;
    p->iz += p->iz_step;
    if (p->iz == p->iz_stop)
      return (false);
  }
  return (-1);
}

static bool	gridHitWithRecord(GridClass *self, RayStruct *ray,
                                  double *tMin, HitRecordStruct *rec)
{
  Poubelle	p;

  gridHitInit(self, &p, ray, tMin, rec);
  gridHita(self, &p);
  gridHitb(self, &p);
  if (!gridHitCheck(self, &p))
    return (false);
  gridHitInitStart(self, &p);
  p.dtx = (p.tx_max - p.tx_min) / ((double)self->nx);
  p.dty = (p.ty_max - p.ty_min) / ((double)self->ny);
  p.dtz = (p.tz_max - p.tz_min) / ((double)self->nz);
  gridInitAll(self, &p);
  while (true)
  {
    int i = p.ix + self->nx * p.iy + self->nx * self->ny * p.iz;
    if (!(i > 0 && i < self->numCells))
      break ;
    p.cellVect =
      self->cells[p.ix + self->nx * p.iy + self->nx * self->ny * p.iz];
    if (p.tx_next < p.ty_next && p.tx_next < p.tz_next)
    {
      if ((p.ret = gridHit1(self, &p)) != -1)
        return (p.ret);
    }
    else
      if ((p.ret = gridHit2(self, &p)) != -1)
        return (p.ret);
  }
  return (false);
}

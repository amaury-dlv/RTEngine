#include "BBox.h"

bool	inside(BBox *bbox, Point3D *p)
{
  return ((p->x > bbox->x0 && p->x < bbox->x1)
       && (p->y > bbox->y0 && p->y < bbox->y1)
       && (p->z > bbox->z0 && p->z < bbox->z1));
}

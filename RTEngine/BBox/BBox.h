#ifndef __BBOX_H__
# define __BBOX_H__

# include "OOC.h"
# include "Point3D.h"

typedef struct BBox	BBox;
struct				BBox
{
  double			x0;
  double			x1;
  double			y0;
  double			y1;
  double			z0;
  double			z1;
};

bool	inside(BBox *bbox, Point3D *p);

#endif /* !__BBOX_H__ */

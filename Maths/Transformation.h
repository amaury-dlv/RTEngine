#ifndef TRANSFORMATION_H_
# define TRANSFORMATION_H_

# include "Point3D.h"
# include "Matrix.h"

void   	translate(Point3D *p, Point3D *mPos);
void	invTranslate(Point3D *p, Point3D *mPos);
void	rotate(Vector3D *p, t_matrix *mMatrix);
void	unrotate(Vector3D *p, t_matrix *mMatrix);

#endif	/* !TRANSFORMATION_H_ */

#ifndef MATRIX_TRANSFORMATION_
# define MATRIX_TRANSFORMATION_

# include "Point3D.h"

typedef struct	s_matrix
{
  Point3D	mRot;
  double	m[3][3][3];
  double	inv_m[3][3][3];
}		t_matrix;

void	setIdentity(t_matrix *mMatrix);
void	composeInvMatrix(t_matrix *mMatrix);
void	composeMatrix(t_matrix *mMatrix);

#endif	/* !MATRIX_TRANSFORMATION_ */

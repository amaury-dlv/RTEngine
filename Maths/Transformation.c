#include "Transformation.h"

void		translate(Point3D *p, Point3D *mPos)
{
  p->x -= mPos->x;
  p->y -= mPos->y;
  p->z -= mPos->z;
}

void		invTranslate(Point3D *p, Point3D *mPos)
{
  p->x += mPos->x;
  p->y += mPos->y;
  p->z += mPos->z;
}

void		rotate(Vector3D *p, t_matrix *mMatrix)
{
  double	s[3];
  int		i;

  i = 0;
  while (i < 3)
  {
    s[0] = p->x;
    s[1] = p->y;
    s[2] = p->z;
    p->x = s[0] * mMatrix->m[i][0][0]
         + s[1] * mMatrix->m[i][0][1]
         + s[2] * mMatrix->m[i][0][2];
    p->y = s[0] * mMatrix->m[i][1][0]
         + s[1] * mMatrix->m[i][1][1]
         + s[2] * mMatrix->m[i][1][2];
    p->z = s[0] * mMatrix->m[i][2][0]
         + s[1] * mMatrix->m[i][2][1]
         + s[2] * mMatrix->m[i][2][2];
    i++;
  }
}

void		unrotate(Vector3D *p, t_matrix *mMatrix)
{
  double	s[3];
  int		i;

  i = 2;
  while (i >= 0)
  {
    s[0] = p->x;
    s[1] = p->y;
    s[2] = p->z;
    p->x = s[0] * mMatrix->inv_m[i][0][0]
         + s[1] * mMatrix->inv_m[i][0][1]
         + s[2] * mMatrix->inv_m[i][0][2];
    p->y = s[0] * mMatrix->inv_m[i][1][0]
         + s[1] * mMatrix->inv_m[i][1][1]
         + s[2] * mMatrix->inv_m[i][1][2];
    p->z = s[0] * mMatrix->inv_m[i][2][0]
         + s[1] * mMatrix->inv_m[i][2][1]
         + s[2] * mMatrix->inv_m[i][2][2];
    i--;
  }
}

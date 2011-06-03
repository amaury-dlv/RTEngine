#include <math.h>

#include "Matrix.h"

void	setIdentity(t_matrix *mMatrix)
{
  int	i;
  int	j;
  int	k;

  i = 0;
  while (i < 3)
  {
    j = 0;
    while (j < 3)
    {
      k = 0;
      while (k < 3)
      {
	mMatrix->m[i][j][k] = mMatrix->inv_m[i][j][k] = 0;
	if (j == k)
	  {
	  mMatrix->m[i][j][k] = 1;
	  mMatrix->inv_m[i][j][k] = 1;
	  }
	k++;
      }
      j++;
    }
    i++;
  }
}

void	composeInvMatrix(t_matrix *mMatrix)
{
  mMatrix->inv_m[0][1][1] = cos(-mMatrix->mRot.x);
  mMatrix->inv_m[0][1][2] = -sin(-mMatrix->mRot.x);
  mMatrix->inv_m[0][2][1] = sin(-mMatrix->mRot.x);
  mMatrix->inv_m[0][2][2] = cos(-mMatrix->mRot.x);
  mMatrix->inv_m[1][0][0] = cos(-mMatrix->mRot.y);
  mMatrix->inv_m[1][0][2] = sin(-mMatrix->mRot.y);
  mMatrix->inv_m[1][2][0] = -sin(-mMatrix->mRot.y);
  mMatrix->inv_m[1][2][2] = cos(-mMatrix->mRot.y);
  mMatrix->inv_m[2][0][0] = cos(-mMatrix->mRot.z);
  mMatrix->inv_m[2][0][1] = -sin(-mMatrix->mRot.z);
  mMatrix->inv_m[2][1][0] = sin(-mMatrix->mRot.z);
  mMatrix->inv_m[2][1][1] = cos(-mMatrix->mRot.z);
}

void	composeMatrix(t_matrix *mMatrix)
{
  mMatrix->m[0][1][1] = cos(mMatrix->mRot.x);
  mMatrix->m[0][1][2] = -sin(mMatrix->mRot.x);
  mMatrix->m[0][2][1] = sin(mMatrix->mRot.x);
  mMatrix->m[0][2][2] = cos(mMatrix->mRot.x);
  mMatrix->m[1][0][0] = cos(mMatrix->mRot.y);
  mMatrix->m[1][0][2] = sin(mMatrix->mRot.y);
  mMatrix->m[1][2][0] = -sin(mMatrix->mRot.y);
  mMatrix->m[1][2][2] = cos(mMatrix->mRot.y);
  mMatrix->m[2][0][0] = cos(mMatrix->mRot.z);
  mMatrix->m[2][0][1] = -sin(mMatrix->mRot.z);
  mMatrix->m[2][1][0] = sin(mMatrix->mRot.z);
  mMatrix->m[2][1][1] = cos(mMatrix->mRot.z);
}

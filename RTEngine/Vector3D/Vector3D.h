#ifndef __VECTOR3D_H__
# define __VECTOR3D_H__

# include "OOC.h"

typedef struct Vector3D	Vector3D;
struct				Vector3D
{
  double			x;
  double			y;
  double			z;
};

double		vectLength(Vector3D* self);
void		vectNormalize(Vector3D* self);
double		vectDist(Vector3D* self, Vector3D* other);
double		vectDistXYZ(Vector3D *self, double x, double y, double z);
Vector3D*	vectSet(Vector3D* self, double x, double y, double z);
int		vectSetFromStr(Vector3D *self, char *str);
Vector3D*	vectOpEqual(Vector3D* self, Vector3D *other);
Vector3D*	vectOpPlus(Vector3D* self, Vector3D *other);
Vector3D*	vectOpMinus(Vector3D* self, Vector3D *other);
Vector3D*	vectOpTimes(Vector3D* self, double a);
Vector3D*	vectOpOver(Vector3D* self, double a);
Vector3D	*vectInv(Vector3D *self);
double		vectDotProduct(Vector3D* self, Vector3D *other);
Vector3D*	vectCrossProduct(Vector3D* self, Vector3D* other);
Vector3D	*vectByMatrix(Vector3D *self, double m[4][4]);
Vector3D	vectReflect(Vector3D *i, Vector3D *n);

#endif /* !__VECTOR3D_H__ */

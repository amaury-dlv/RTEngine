#include <stdlib.h>
#include <math.h>

#ifndef __MATHS_H__
# define __MATHS_H__

# include "Point3D.h"

# define MAX(a, b)	(((a) > (b)) ? (a) : (b))
# define MIN(a, b)	(((a) < (b)) ? (a) : (b))
# define ABS(x)		(((x) > 0) ? (x) : -(x))

# ifndef M_PI
#  define M_PI 3.141592653589793238462643
# endif

extern const double 	Pi;
extern const double 	PiOn180;
extern const double 	TwoPi;
extern const double	Infinity;
extern const double	kEpsilon;
extern const double	ShadowKEpsilon;
extern const double	InvPi;
extern const double	InvTwoPi;
extern const double	DeathNumber;

int	rand_int(void);
float	rand_float(void);
float	rand_float2(int l, float h);

void	toRad(Point3D *mRot);

#define		EQN_EPS		1e-90
#define		IsZero(x)	((x) > -EQN_EPS && (x) < EQN_EPS)

typedef struct	s_qsolve
{
  int		i;
  int		num;
  double	sub;
  double	a;
  double	b;
  double	c;
  double	sq_a;
  double	p;
  double	q;
  double	cb_p;
  double	d;
  double	u;
  double	phi;
  double	t;
  double	sqrt_d;
  double	v;
}		t_qsolve;

typedef struct	s_squartic
{
  double	coeffs[4];
  double	z;
  double	u;
  double	v;
  double	sub;
  double	a;
  double	b;
  double	c;
  double	d;
  double	sq_a;
  double	p;
  double	q;
  double	r;
  int		i;
  int		num;
}		t_squartic;

void	set_m(t_qsolve *m, double c[4]);
void	ifzero(t_qsolve *m, double s[3]);
void	solve_cubic_else(t_qsolve *m, double s[3]);
int	solve_cubic(double c[4], double s[3]);
int	solve_quadric(double c[3], double s[2]);
void	init_solve_quartic(t_squartic *m, double c[5]);
int	return_zero_if_isnt_zero(t_squartic *m);
int	isnt_zero(t_squartic *m, double s[4]);
int	solve_quartic(double c[5], double s[4]);
double	clamp(double x, double min, double max);
float	step(float edge, float x);
float	smoothstep(float edge0, float edge1, float x);

#endif	/* !__MATHS_H__ */

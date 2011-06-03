#include "Maths.h"

int		solve_quadric(double c[3], double s[2])
{
  double	p;
  double	q;
  double	d;
  double	sqrt_d;

  p = c[1] / (2 * c[2]);
  q = c[0] / c[2];
  d = p * p - q;
  if (IsZero(d))
    {
      s[0] = - p;
      return (1);
    }
  else if (d > 0)
    {
      sqrt_d = sqrt(d);
      s[0] = sqrt_d - p;
      s[1] = -sqrt_d - p;
      return (2);
    }
  else
    return (0);
}

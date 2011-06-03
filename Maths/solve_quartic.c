#include "Maths.h"

int		solve_quartic(double c[5], double s[4])
{
  t_squartic	m;

  init_solve_quartic(&m, c);
  if (IsZero(m.r))
    {
      m.coeffs[0] = m.q;
      m.coeffs[1] = m.p;
      m.coeffs[2] = 0;
      m.coeffs[3] = 1;
      m.num = solve_cubic(m.coeffs, s);
      s[m.num++] = 0;
    }
  else
    if (isnt_zero(&m, s) == 0)
      return (0);
  m.sub = 1.0 / 4 * m.a;
  m.i = 0;
  while (m.i < m.num)
    {
      s[m.i] -= m.sub;
      ++m.i;
    }
  return (m.num);
}

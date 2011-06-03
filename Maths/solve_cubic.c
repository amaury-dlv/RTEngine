#include "Maths.h"

int		solve_cubic(double c[4], double s[3])
{
  int		i;
  t_qsolve	m;

  set_m(&m, c);
  if (IsZero(m.d))
    ifzero(&m, s);
  else if (m.d < 0)
    {
      m.phi = 1.0 / 3 * acos(-m.q / sqrt(-m.cb_p));
      m.t = 2 * sqrt(-m.p);
      s[0] = m.t * cos(m.phi);
      s[1] = -m.t * cos(m.phi + M_PI / 3);
      s[2] = -m.t * cos(m.phi - M_PI / 3);
      m.num = 3;
    }
  else
    solve_cubic_else(&m, s);
  m.sub = 1.0 / 3 * m.a;
  i = 0;
  while (i < m.num)
    {
      s[i] -= m.sub;
      ++i;
    }
  return (m.num);
}

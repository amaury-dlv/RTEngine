#include "Maths.h"

void		solve_cubic_else(t_qsolve *m, double s[3])
{
  m->sqrt_d = sqrt(m->d);
  m->u = cbrt(m->sqrt_d - m->q);
  m->v = -cbrt(m->sqrt_d + m->q);
  s[0] = m->u + m->v;
  m->num = 1;
}

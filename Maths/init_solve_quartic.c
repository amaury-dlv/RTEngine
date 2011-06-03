#include "Maths.h"

void		init_solve_quartic(t_squartic *m, double c[5])
{
  m->a = c[3] / c[4];
  m->b = c[2] / c[4];
  m->c = c[1] / c[4];
  m->d = c[0] / c[4];
  m->sq_a = m->a * m->a;
  m->p = - 3.0 / 8 * m->sq_a + m->b;
  m->q = 1.0 / 8 * m->sq_a * m->a - 1.0 / 2 * m->a * m->b + m->c;
  m->r = - 3.0 / 256 * m->sq_a * m->sq_a + 1.0 / 16 * m->sq_a *
    m->b - 1.0 / 4 * m->a * m->c + m->d;
}

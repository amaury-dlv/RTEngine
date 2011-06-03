#include "Maths.h"

void		set_m(t_qsolve *m, double c[4])
{
  m->a = c[2] / c[3];
  m->b = c[1] / c[3];
  m->c = c[0] / c[3];
  m->sq_a = m->a * m->a;
  m->p = 1.0 / 3 * (- 1.0 / 3 * m->sq_a + m->b);
  m->q = 1.0 / 2 * (2.0 / 27 * m->a * m->sq_a - 1.0 / 3 * m->a * m->b + m->c);
  m->cb_p = m->p * m->p * m->p;
  m->d = m->q * m->q + m->cb_p;
}

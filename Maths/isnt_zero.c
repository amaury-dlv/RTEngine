#include "Maths.h"

int		isnt_zero(t_squartic *m, double s[4])
{
  m->coeffs[0] = 1.0 / 2 * m->r * m->p - 1.0 / 8 * m->q * m->q;
  m->coeffs[1] = -m->r;
  m->coeffs[2] = - 1.0 / 2 * m->p;
  m->coeffs[3] = 1;
  (void)solve_cubic(m->coeffs, s);
  m->z = s[0];
  m->u = m->z * m->z - m->r;
  m->v = 2 * m->z - m->p;
  if (return_zero_if_isnt_zero(m) == 0)
    return (0);
  m->coeffs[0] = m->z - m->u;
  m->coeffs[1] = m->q < 0 ? -m->v : m->v;
  m->coeffs[2] = 1;
  m->num = solve_quadric(m->coeffs, s);
  m->coeffs[0] = m->z + m->u;
  m->coeffs[1] = m->q < 0 ? m->v : -m->v;
  m->coeffs[2] = 1;
  m->num += solve_quadric(m->coeffs, s + m->num);
  return (1);
}

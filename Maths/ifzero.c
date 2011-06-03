#include "Maths.h"

void		ifzero(t_qsolve *m, double s[3])
{
  if (IsZero(m->q))
    {
      s[0] = 0;
      m->num = 1;
    }
  else
    {
      m->u = cbrt(-m->q);
      s[0] = 2 * m->u;
      s[1] = - m->u;
      m->num = 2;
    }
}

#include "Maths.h"

int		return_zero_if_isnt_zero(t_squartic *m)
{
  if (IsZero(m->u))
    m->u = 0;
  else if (m->u > 0)
    m->u = sqrt(m->u);
  else
    return (0);
  if (IsZero(m->v))
    m->v = 0;
  else if (m->v > 0)
    m->v = sqrt(m->v);
  else
    return (0);
  return (1);
}

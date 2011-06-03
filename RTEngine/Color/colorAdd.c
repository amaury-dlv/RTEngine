#include "Color.h"
#include "Maths.h"
#include "Utils.h"

Color		*colorAdd(Color *lhs, Color *rhs)
{
  lhs->r += rhs->r;
  lhs->g += rhs->g;
  lhs->b += rhs->b;
  return (lhs);
}


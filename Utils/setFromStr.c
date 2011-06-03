#include <stdlib.h>
#include <stdio.h>

#include "Utils.h"

int		setFloatFromStr(char *str, float *n)
{
  char		*t;

  t = str;
  *n = strtof(str, &str);
  if (t == str)
  {
    fprintf(stderr, "Expected a floating number.\n");
    return (-1);
  }
  return (0);
}

int		setDoubleFromStr(char *str, double *n)
{
  char		*t;

  t = str;
  *n = strtod(str, &str);
  if (t == str)
  {
    fprintf(stderr, "Expected a floating number.\n");
    return (-1);
  }
  return (0);
}

int		setIntFromStr(char *str, int *n)
{
  char		*t;

  t = str;
  *n = strtol(str, &str, 10);
  if (t == str)
  {
    fprintf(stderr, "Expected a number.\n");
    return (-1);
  }
  return (0);
}

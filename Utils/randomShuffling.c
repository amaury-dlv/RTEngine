#include <stdlib.h>

void		randomShuffle(int *array, int n)
{
  int		i;

  if (n > 1) 
  {
    i = 0;
    while (i < n - 1)
    {
      int j = i + rand() / (RAND_MAX / (n - i) + 1);
      int t = array[j];
      array[j] = array[i];
      array[i] = t;
      i++;
    }
  }
}


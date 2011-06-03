#include <string.h>
#include <stdlib.h>

#include "Utils.h"

static int	is_alnum(char c, char *del)
{
  int		i;

  i = 0;
  while (del[i])
    if (c == del[i++])
      return (0);
  return (1 && c != '\0');
}

static int	wordlen(char *str, char *del)
{
  int		len;

  len = 0;
  while (is_alnum(str[len], del))
    len++;
  return (len);
}

static int	count_words(char *str, char *del)
{
  int		i;
  int		n;

  n = 0;
  i = 0;
  while (str[i] != '\0')
  {
    n += (i == 0 && is_alnum(str[i], del));
    n += (i > 0 && !is_alnum(str[i - 1], del) && is_alnum(str[i], del));
    i++;
  }
  return (n);
}

static char	*get_word(char *str, char *del)
{
  static int	i = 0;
  char		*w;
  int		j;

  if (str == NULL)
  {
    i = 0;
    return (NULL);
  }
  while (!is_alnum(str[i], del))
    i++;
  w = xmalloc(wordlen(&str[i], del) + 1);
  j = 0;
  while (is_alnum(str[i], del))
    w[j++] = str[i++];
  w[j] = '\0';
  return (w);
}

char	**toWordtab(char *str, char *del)
{
  char	**tab;
  int	iw;
  int	nwords;

  get_word(NULL, del);
  nwords = count_words(str, del);
  tab = xmalloc((nwords + 1) * sizeof(*tab));
  iw = 0;
  while (iw < nwords)
  {
    tab[iw] = get_word(str, del);
    iw++;
  }
  tab[iw] = NULL;
  return (tab);
}

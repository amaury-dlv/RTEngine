#ifndef __UTILS_H__
# define __UTILS_H__

void		*xmalloc(int size);
void		*xrealloc(void *ptr, int size);
void		*my_realloc(void *ptr, int size);
void		*my_memset(void *begin, int c, int size);
int		setFloatFromStr(char *str, float *n);
int		setDoubleFromStr(char *str, double *n);
int		setIntFromStr(char *str, int *n);
char		**toWordtab(char *str, char *del);
void		freeWordtab(char **tab);
int		wordtabSize(char **wtab);
void 		randomShuffle(int *array, int n);

#endif /* !__UTILS_H__ */

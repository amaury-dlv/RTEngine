#ifndef		__RAISE_H__
# define	__RAISE_H__

# define	raise(m, ...)	\
                raise_int(__FILE__, __LINE__, m, ##__VA_ARGS__)

void	raise_int(char *const file, int line, char *msg, ...);

#endif		/* !__RAISE_H__ */

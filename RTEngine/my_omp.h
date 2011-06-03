#ifndef MY_OMP_H_
# define MY_OMP_H_

#ifdef USE_THREADS
# include <omp.h>
# define OMP_NUM()	omp_get_thread_num()
#else
# define OMP_NUM()	0
#endif

#endif /* !MY_OMP_H_ */

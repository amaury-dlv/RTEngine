#ifndef __PERLIN_NOISE_H__
# define __PERLIN_NOISE_H__

void		init_noise() __attribute__((constructor));
double		pnoise(double x, double y, double z);
double		fade(double t);
double		lerp(double t, double a, double b);
double		grad(int hash, double x, double y, double z);

#endif	/* !__PERLIN_NOISE_H__ */

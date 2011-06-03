#ifndef __COLOR_H__
# define __COLOR_H__

# define COLORPTR(x)	((Color*)&((Color[]){(Color)(x)}))

typedef struct	Color
{
  float		r;
  float		g;
  float		b;
}		Color;

unsigned int		colorGetUInt(Color* color);
Color			*colorMult(Color *color, float k);
Color			*colorMultColor(Color *color, Color *k);
Color			*colorAdd(Color *lhs, Color *rhs);
Color			*colorAddFloat(Color *color, float k);
int			setColorFromStr(char *str, Color *color);

#endif /* !__COLOR_H__ */

#ifndef __RENDERPASS_H__
# define __RENDERPASS_H__

# include "OOC.h"

# define	RENDERPASS(x)	((RenderPassClass *)(x))

typedef struct RenderPassClass	RenderPassClass;
struct                          RenderPassClass
{
  /* SuperClass : */
  Class				__base__;
  /* public : */
  void				(*init)();
  void				(*apply)(RenderPassClass *self);
  void				(*applyLoop)(RenderPassClass *self);
  void				(*applyPixel)(RenderPassClass *self, int x, int y);
  void				(*print)(RenderPassClass *self, int current, int total);
  void				(*updateCurrentState)(RenderPassClass *self);
};

extern Class*			RenderPass;

int	renderPassUnserializeSet();

#endif /* !__RENDERPASS_H__ */

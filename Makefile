##
## Makefile for rt in /
##
## Made by amaury patard-de-la-vieuville
## Login   <patard_a@epitech.net>
##
## Started on  Sun Dec 12 17:45:44 2010 amaury patard-de-la-vieuville
## Last update Sun Dec 12 17:45:44 2010 amaury patard-de-la-vieuville
##

RNAME =		rt

ifndef USE_THREADS
USE_THREADS =	1
endif

DEPS_NAME = 	Makefile.dep

RSRCS =		main.c \
		Maths/clamp.c \
		Maths/ifzero.c \
		Maths/init_solve_quartic.c \
		Maths/isnt_zero.c \
		Maths/Maths.c \
		Maths/Matrix.c \
		Maths/PerlinNoise.c \
		Maths/PerlinNoiseMisc.c \
		Maths/rand_float2.c \
		Maths/rand_float.c \
		Maths/rand_int.c \
		Maths/return_zero_if_isnt_zero.c \
		Maths/set_m.c \
		Maths/smoothstep.c \
		Maths/solve_cubic.c \
		Maths/solve_cubic_else.c \
		Maths/solve_quadric.c \
		Maths/solve_quartic.c \
		Maths/step.c \
		Maths/toRad.c \
		Maths/Transformation.c \
		OOC/Dequeue/Dequeue.c \
		OOC/new.c \
		OOC/raise.c \
		OOC/Vector/Vector.c \
		RTEngine/BBox/BBox.c \
		RTEngine/BRDF/BRDF.c \
		RTEngine/BRDF/GlossySpecular/GlossySpecular.c \
		RTEngine/BRDF/Lambertian/Lambertian.c \
		RTEngine/BRDF/PerfectSpecular/PerfectSpecular.c \
		RTEngine/BTDF/BTDF.c \
		RTEngine/BTDF/PerfectTransmitter/PerfectTransmitter.c \
		RTEngine/Buffer/GBuffer/GBuffer.c \
		RTEngine/Buffer/ImageBuffer/BoolBuffer/BoolBuffer.c \
		RTEngine/Buffer/ImageBuffer/ColorBuffer/ColorBuffer.c \
		RTEngine/Buffer/ImageBuffer/DepthBuffer/DepthBuffer.c \
		RTEngine/Buffer/ImageBuffer/ImageBuffer.c \
		RTEngine/Buffer/ImageBuffer/LightBuffer/LightBuffer.c \
		RTEngine/Buffer/ImageBuffer/MaterialBuffer/MaterialBuffer.c \
		RTEngine/Buffer/ImageBuffer/NormalBuffer/NormalBuffer.c \
		RTEngine/Buffer/ImageBuffer/PointBuffer/PointBuffer.c \
		RTEngine/Buffer/ImageBuffer/RayBuffer/RayBuffer.c \
		RTEngine/Camera/Camera.c \
		RTEngine/Camera/FishEye/FishEye.c \
		RTEngine/Camera/Pinhole/Pinhole.c \
		RTEngine/Camera/Spherical/Spherical.c \
		RTEngine/Cluster/Cluster.c \
		RTEngine/Cluster/ClusterClient/ClusterClient.c \
		RTEngine/Cluster/ClusterServer/ClusterServer.c \
		RTEngine/Color/colorAdd.c \
		RTEngine/Color/colorAddFloat.c \
		RTEngine/Color/colorGetCampled.c \
		RTEngine/Color/colorGetUInt.c \
		RTEngine/Color/colorMult.c \
		RTEngine/Color/colorMultColor.c \
		RTEngine/Color/setColorFromStr.c \
		RTEngine/GeometricObject/Box/Box.c \
		RTEngine/GeometricObject/Cone/Cone.c \
		RTEngine/GeometricObject/CubeTroue/CubeTroue.c \
		RTEngine/GeometricObject/Cylinder/Cylinder.c \
		RTEngine/GeometricObject/Disc/Disc.c \
		RTEngine/GeometricObject/GeometricObject.c \
		RTEngine/GeometricObject/Grid/Grid.c \
		RTEngine/GeometricObject/MeshTriangle/FlatMeshTriangle/FlatMeshTriangle.c \
		RTEngine/GeometricObject/MeshTriangle/MeshTriangle.c \
		RTEngine/GeometricObject/MeshTriangle/SmoothMeshTriangle/SmoothMeshTriangle.c \
		RTEngine/GeometricObject/Plane/Plane.c \
		RTEngine/GeometricObject/Rectangle/Rectangle.c \
		RTEngine/GeometricObject/Sphere/Sphere.c \
		RTEngine/GeometricObject/Tore/Tore.c \
		RTEngine/GraphicsManager/DummyGraphicsManager/DummyGraphicsManager.c \
		RTEngine/GraphicsManager/GraphicsManager.c \
		RTEngine/HitRecord/HitRecord.c \
		RTEngine/Light/Ambient/Ambient.c \
		RTEngine/Light/AreaLight/AreaLight.c \
		RTEngine/Light/Directional/Directional.c \
		RTEngine/Light/EnvironmentLight/EnvironmentLight.c \
		RTEngine/Light/FakeSphericalLight/FakeSphericalLight.c \
		RTEngine/Light/Light.c \
		RTEngine/Light/PointLight/PointLight.c \
		RTEngine/Material/Emissive/Emissive.c \
		RTEngine/Material/GlossyReflective/GlossyReflective.c \
		RTEngine/Material/Material.c \
		RTEngine/Material/Matte/Matte.c \
		RTEngine/Material/Phong/ImageTexture/ImageTexture.c \
		RTEngine/Material/Phong/Noise/Noise.c \
		RTEngine/Material/Phong/Phong.c \
		RTEngine/Material/Reflective/Reflective.c \
		RTEngine/Material/Transparent/Bump/Bump.c \
		RTEngine/Material/Transparent/Checker/Checker.c \
		RTEngine/Material/Transparent/Transparent.c \
		RTEngine/Mesh/Mesh.c \
		RTEngine/Mesh/plyfile.c \
		RTEngine/OptionsManager/OptionsManager.c \
		RTEngine/Point/Point3D.c \
		RTEngine/Ray/Ray.c \
		RTEngine/RenderingScene/RenderingScene.c \
		RTEngine/RenderPass/AORenderPass/AORenderPass.c \
		RTEngine/RenderPass/AreaLightRenderPass/AreaLightRenderPass.c \
		RTEngine/RenderPass/BumpRenderPass/BumpRenderPass.c \
		RTEngine/RenderPass/DOFRenderPass/DOFRenderPass.c \
		RTEngine/RenderPass/FirstRenderPass/FirstRenderPass.c \
		RTEngine/RenderPass/GammaRenderPass/GammaRenderPass.c \
		RTEngine/RenderPass/InstRadiosityRenderPass/InstRadiosityRenderPass.c \
		RTEngine/RenderPass/LightRenderPass/LightRenderPass.c \
		RTEngine/RenderPass/NegativeRenderPass/NegativeRenderPass.c \
		RTEngine/RenderPass/OversamplingRenderPass/OversamplingRenderPass.c \
		RTEngine/RenderPass/PathTracingRenderPass/PathTracingRenderPass.c \
		RTEngine/RenderPass/ReflectionRenderPass/ReflectionRenderPass.c \
		RTEngine/RenderPass/RenderPass.c \
		RTEngine/RenderPass/ShadeRGBRenderPass/ShadeRGBRenderPass.c \
		RTEngine/RenderPass/ShadowRenderPass/ShadowRenderPass.c \
		RTEngine/RenderPass/SSAORenderPass/SSAORenderPass.c \
		RTEngine/RenderPass/TextureRenderPass/TextureRenderPass.c \
		RTEngine/RenderPass/ToneMappingRenderPass/ToneMappingRenderPass.c \
		RTEngine/RenderPass/TransparencyRenderPass/TransparencyRenderPass.c \
		RTEngine/RTEngine.c \
		RTEngine/Sampling/Sampler/Jittered/Jittered.c \
		RTEngine/Sampling/SamplerManager/SamplerManager.c \
		RTEngine/Sampling/Sampler/MultiJittered/MultiJittered.c \
		RTEngine/Sampling/Sampler/PureRandom/PureRandom.c \
		RTEngine/Sampling/Sampler/Regular/Regular.c \
		RTEngine/Sampling/Sampler/Sampler.c \
		RTEngine/Scene/Scene.c \
		RTEngine/Tracing/Tracer/AreaLighting/AreaLighting.c \
		RTEngine/Tracing/TracerManager/TracerManager.c \
		RTEngine/Tracing/Tracer/PathTracer/PathTracer.c \
		RTEngine/Tracing/Tracer/Raycast/Raycast.c \
		RTEngine/Tracing/Tracer/Tracer.c \
		RTEngine/Tracing/Tracer/Whitted/Whitted.c \
		RTEngine/Vector3D/vect3DSet.c \
		RTEngine/Vector3D/vectByMatrix.c \
		RTEngine/Vector3D/vectCrossProduct.c \
		RTEngine/Vector3D/vectDist.c \
		RTEngine/Vector3D/vectDistXYZ.c \
		RTEngine/Vector3D/vectDotProduct.c \
		RTEngine/Vector3D/vectInv.c \
		RTEngine/Vector3D/vectLenght.c \
		RTEngine/Vector3D/vectNormalize.c \
		RTEngine/Vector3D/vectOpEqual.c \
		RTEngine/Vector3D/vectOpMinus.c \
		RTEngine/Vector3D/vectOpOver.c \
		RTEngine/Vector3D/vectOpPlus.c \
		RTEngine/Vector3D/vectOpTimes.c \
		RTEngine/Vector3D/vectReflect.c \
		RTEngine/Vector3D/vectSetFromStr.c \
		RTEngine/XML/SceneSerializer/SceneSerializer.c \
		RTEngine/XML/XMLDocument/XMLDocument.c \
		Utils/freeWordtab.c \
		Utils/randomShuffling.c \
		Utils/setFromStr.c \
		Utils/toWordtab.c \
		Utils/wordtabSize.c \
		Utils/xmalloc.c \
		Utils/xrealloc.c

ifndef NO_GRAPHICS
RSRCS +=	RTEngine/GraphicsManager/BmpGraphicsManager/BmpGraphicsManager.c

ifdef HAS_SDL
RSRCS +=	RTEngine/GraphicsManager/SdlGraphicsManager/SdlGraphicsManager.c
endif # HAS_SDL

ifdef HAS_MLX
RSRCS +=	RTEngine/GraphicsManager/MlxGraphicsManager/MlxGraphicsManager.c
endif # HAS_MLX

endif # NO_GRAPHICS

ROBJS =		$(RSRCS:.c=.o)

INCDIRS =	OOC/ \
		OOC/Vector/ \
		OOC/Dequeue/ \
		Maths/ \
		RTEngine/ \
		RTEngine/BBox/ \
		RTEngine/BRDF/ \
		RTEngine/BRDF/GlossySpecular/ \
		RTEngine/BRDF/Lambertian/ \
		RTEngine/BRDF/PerfectSpecular/ \
		RTEngine/BTDF/ \
		RTEngine/BTDF/PerfectTransmitter/ \
		RTEngine/Buffer/ \
		RTEngine/Buffer/GBuffer/ \
		RTEngine/Buffer/ImageBuffer/ \
		RTEngine/Buffer/ImageBuffer/BoolBuffer/ \
		RTEngine/Buffer/ImageBuffer/ColorBuffer/ \
		RTEngine/Buffer/ImageBuffer/DepthBuffer/ \
		RTEngine/Buffer/ImageBuffer/LightBuffer/ \
		RTEngine/Buffer/ImageBuffer/MaterialBuffer/ \
		RTEngine/Buffer/ImageBuffer/NormalBuffer/ \
		RTEngine/Buffer/ImageBuffer/PointBuffer/ \
		RTEngine/Buffer/ImageBuffer/RayBuffer/ \
		RTEngine/Camera/ \
		RTEngine/Camera/FishEye/ \
		RTEngine/Camera/Pinhole/ \
		RTEngine/Camera/Spherical/ \
		RTEngine/Cluster/ \
		RTEngine/Cluster/ClusterClient/ \
		RTEngine/Cluster/ClusterServer/ \
		RTEngine/Color/ \
		RTEngine/GeometricObject/ \
		RTEngine/GeometricObject/Box/ \
		RTEngine/GeometricObject/Cone/ \
		RTEngine/GeometricObject/CubeTroue/ \
		RTEngine/GeometricObject/Cylinder/ \
		RTEngine/GeometricObject/Disc/ \
		RTEngine/GeometricObject/Grid/ \
		RTEngine/GeometricObject/MeshTriangle/ \
		RTEngine/GeometricObject/MeshTriangle/FlatMeshTriangle/ \
		RTEngine/GeometricObject/MeshTriangle/SmoothMeshTriangle/ \
		RTEngine/GeometricObject/Plane/ \
		RTEngine/GeometricObject/Rectangle/ \
		RTEngine/GeometricObject/Sphere/ \
		RTEngine/GeometricObject/Tore/ \
		RTEngine/GraphicsManager/ \
		RTEngine/GraphicsManager/BmpGraphicsManager/ \
		RTEngine/GraphicsManager/DummyGraphicsManager/ \
		RTEngine/GraphicsManager/MlxGraphicsManager/ \
		RTEngine/GraphicsManager/SdlGraphicsManager/ \
		RTEngine/HitRecord/ \
		RTEngine/Light/ \
		RTEngine/Light/Ambient/ \
		RTEngine/Light/AreaLight/ \
		RTEngine/Light/Directional/ \
		RTEngine/Light/EnvironmentLight/ \
		RTEngine/Light/FakeSphericalLight/ \
		RTEngine/Light/PointLight/ \
		RTEngine/Material/ \
		RTEngine/Material/Emissive/ \
		RTEngine/Material/GlossyReflective/ \
		RTEngine/Material/Matte/ \
		RTEngine/Material/Phong/ \
		RTEngine/Material/Phong/ImageTexture/ \
		RTEngine/Material/Phong/Noise/ \
		RTEngine/Material/Reflective/ \
		RTEngine/Material/Transparent/ \
		RTEngine/Material/Transparent/Bump/ \
		RTEngine/Material/Transparent/Checker/ \
		RTEngine/Mesh/ \
		RTEngine/OptionsManager/ \
		RTEngine/Point/ \
		RTEngine/Ray/ \
		RTEngine/RenderingScene/ \
		RTEngine/RenderPass/ \
		RTEngine/RenderPass/AORenderPass/ \
		RTEngine/RenderPass/AreaLightRenderPass/ \
		RTEngine/RenderPass/BumpRenderPass/ \
		RTEngine/RenderPass/DOFRenderPass/ \
		RTEngine/RenderPass/FirstRenderPass/ \
		RTEngine/RenderPass/GammaRenderPass/ \
		RTEngine/RenderPass/InstRadiosityRenderPass/ \
		RTEngine/RenderPass/LightRenderPass/ \
		RTEngine/RenderPass/NegativeRenderPass/ \
		RTEngine/RenderPass/OversamplingRenderPass/ \
		RTEngine/RenderPass/PathTracingRenderPass/ \
		RTEngine/RenderPass/ReflectionRenderPass/ \
		RTEngine/RenderPass/ShadeRGBRenderPass/ \
		RTEngine/RenderPass/ShadowRenderPass/ \
		RTEngine/RenderPass/SSAORenderPass/ \
		RTEngine/RenderPass/TextureRenderPass/ \
		RTEngine/RenderPass/ToneMappingRenderPass/ \
		RTEngine/RenderPass/TransparencyRenderPass/ \
		RTEngine/Sampling/ \
		RTEngine/Sampling/Sampler/ \
		RTEngine/Sampling/Sampler/Hammersley/ \
		RTEngine/Sampling/Sampler/Jittered/ \
		RTEngine/Sampling/SamplerManager/ \
		RTEngine/Sampling/Sampler/MultiJittered/ \
		RTEngine/Sampling/Sampler/NRooks/ \
		RTEngine/Sampling/Sampler/PureRandom/ \
		RTEngine/Sampling/Sampler/Regular/ \
		RTEngine/Scene/ \
		RTEngine/Tracing/ \
		RTEngine/Tracing/Tracer/ \
		RTEngine/Tracing/Tracer/AreaLighting/ \
		RTEngine/Tracing/TracerManager/ \
		RTEngine/Tracing/Tracer/PathTracer/ \
		RTEngine/Tracing/Tracer/Raycast/ \
		RTEngine/Tracing/Tracer/Whitted/ \
		RTEngine/Vector3D/ \
		RTEngine/XML/SceneSerializer/ \
		RTEngine/XML/XMLDocument/ \
		Utils/ \
		/usr/include/libxml2

INCFLAGS =	$(addprefix -I,$(INCDIRS))

CC =		gcc

LDFLAGS =	-lm -lxml2

ifdef NO_GRAPHICS
CFLAGS += -DNO_GRAPHICS=1
else

ifdef HAS_SDL
CFLAGS += -DHAS_SDL=1
endif

ifdef HAS_MLX
CFLAGS += -DHAS_MLX=1
endif

ifdef HAS_MLX
LDFLAGS += -lX11 -lXext -lmlx
endif # HAS_MLX

ifdef HAS_SDL
ifdef OSX
LDFLAGS +=      `sdl-config --cflags --libs`
else
LDFLAGS +=	-lSDL
endif # OSX
endif # HAS_SDL
endif # NO_GRAPHICS

ifdef SPEED
CFLAGs += -O3
else
CFLAGS += -g3
endif

CFLAGS +=	-W -Wall -Wextra
CFLAGS +=	-pthread
ifeq ($(USE_THREADS), 1)
CFLAGS +=	-DUSE_THREADS -fopenmp
LDFLAGS +=	-fopenmp
endif # USE_THREADS

ifeq ($(FASTPT), 1)
CFLAGS +=	-DFASTPT
endif

RM =		rm -f

$(RNAME) :	$(ROBJS)
		$(CC) -o $(RNAME) $(ROBJS) $(LDFLAGS)

all :		$(RNAME)

clean :
		$(RM) $(ROBJS) $(DEPS_NAME)

fclean :	clean
		$(RM) $(RNAME)

re :		fclean all

.c.o :
		@$(CC) $(CFLAGS) -c $< -o $(<:.c=.o) $(INCFLAGS)
		@echo "\033[32m[OK]\033[m $<"

$(DEPS_NAME) :	$(RSRCS) $(INCDIRS)
		@printf "\033[34mUpdating Makefile.dep\033[m\n"
		$(RM) $@
		@for file in $(RSRCS); \
			do gcc $(addprefix -I,$(INCDIRS)) \
			    -MT `echo $$file | sed 's/\.c/\.o/g'` \
			    -MM $$file >> $@;\
		done
		@printf "Done.\n"

deps :		$(DEPS_NAME)
$(ROBJS) :	Makefile
-include	$(DEPS_NAME)

.PHONY :	clean fclean all re deps

SDL_TTF_DIR = $(NXDK_DIR)/lib/sdl/SDL_ttf
SDL_TTF_SRCS += $(SDL_TTF_DIR)/SDL_ttf.c
SDL_TTF_OBJS = $(addsuffix .obj, $(basename $(SDL_TTF_SRCS)))

$(NXDK_DIR)/lib/libSDL_ttf.lib: $(SDL_TTF_OBJS)

FREETYPE_DIR = $(NXDK_DIR)/lib/sdl/SDL_ttf/external/freetype-2.4.12
FREETYPE_SRCS += $(FREETYPE_DIR)/src/base/ftbase.c
FREETYPE_SRCS += $(FREETYPE_DIR)/src/base/ftglyph.c
FREETYPE_SRCS += $(FREETYPE_DIR)/src/base/ftbitmap.c
FREETYPE_SRCS += $(FREETYPE_DIR)/src/base/ftstroke.c
FREETYPE_SRCS += $(FREETYPE_DIR)/src/base/ftinit.c
FREETYPE_SRCS += $(FREETYPE_DIR)/src/base/ftsystem.c
FREETYPE_SRCS += $(FREETYPE_DIR)/src/truetype/truetype.c
FREETYPE_SRCS += $(FREETYPE_DIR)/src/raster/raster.c
FREETYPE_SRCS += $(FREETYPE_DIR)/src/smooth/smooth.c
FREETYPE_SRCS += $(FREETYPE_DIR)/src/autofit/autofit.c
FREETYPE_SRCS += $(FREETYPE_DIR)/src/sfnt/sfnt.c
FREETYPE_OBJS = $(addsuffix .obj, $(basename $(FREETYPE_SRCS)))

$(NXDK_DIR)/lib/libfreetype.lib: $(FREETYPE_OBJS)

NXDK_CFLAGS += -I$(NXDK_DIR)/lib/sdl \
               -I$(SDL_TTF_DIR) \
               -I$(FREETYPE_DIR)/include \
               -DXBOX

NXDK_CXXFLAGS += -I$(NXDK_DIR)/lib/sdl \
                 -I$(SDL_TTF_DIR) \
                 -I$(FREETYPE_DIR)/include \
                 -DXBOX

main.exe: $(NXDK_DIR)/lib/libSDL_ttf.lib $(NXDK_DIR)/lib/libfreetype.lib

CLEANRULES += clean-sdl_ttf

.PHONY: clean-sdl_ttf
clean-sdl_ttf:
	$(VE)rm -f $(SDL_TTF_OBJS) \
	           $(FREETYPE_OBJS) \
	           $(NXDK_DIR)/lib/libSDL_ttf.lib \
	           $(NXDK_DIR)/lib/libfreetype.lib

LIBSDLIMAGE_DIR = $(NXDK_DIR)/lib/sdl/SDL2_image

LIBSDLIMAGE_SRCS = $(LIBSDLIMAGE_DIR)/IMG.c \
                   $(LIBSDLIMAGE_DIR)/IMG_png.c \
                   $(LIBSDLIMAGE_DIR)/IMG_bmp.c \
                   $(LIBSDLIMAGE_DIR)/IMG_gif.c \
                   $(LIBSDLIMAGE_DIR)/IMG_jpg.c \
                   $(LIBSDLIMAGE_DIR)/IMG_lbm.c \
                   $(LIBSDLIMAGE_DIR)/IMG_pcx.c \
                   $(LIBSDLIMAGE_DIR)/IMG_pnm.c \
                   $(LIBSDLIMAGE_DIR)/IMG_svg.c \
                   $(LIBSDLIMAGE_DIR)/IMG_tga.c \
                   $(LIBSDLIMAGE_DIR)/IMG_tif.c \
                   $(LIBSDLIMAGE_DIR)/IMG_webp.c \
                   $(LIBSDLIMAGE_DIR)/IMG_WIC.c \
                   $(LIBSDLIMAGE_DIR)/IMG_xcf.c \
                   $(LIBSDLIMAGE_DIR)/IMG_xpm.c \
                   $(LIBSDLIMAGE_DIR)/IMG_xv.c \
                   $(LIBSDLIMAGE_DIR)/IMG_xxx.c

LIBSDLIMAGE_OBJS = $(addsuffix .obj, $(basename $(LIBSDLIMAGE_SRCS)))
NXDK_CFLAGS += -I$(NXDK_DIR)/lib/sdl/SDL2_image -DLOAD_BMP -DLOAD_GIF -DLOAD_JPG -DLOAD_LBM -DLOAD_PCX -DLOAD_PNG -DLOAD_PNM -DLOAD_TGA -DLOAD_XCF -DLOAD_XPM -DLOAD_XV -DLOAD_XXX
NXDK_CXXFLAGS += -I$(NXDK_DIR)/lib/sdl/SDL2_image -DLOAD_BMP -DLOAD_GIF -DLOAD_JPG -DLOAD_LBM -DLOAD_PCX -DLOAD_PNG -DLOAD_PNM -DLOAD_TGA -DLOAD_XCF -DLOAD_XPM -DLOAD_XV -DLOAD_XXX

$(NXDK_DIR)/lib/libSDL2_image.lib: $(LIBSDLIMAGE_OBJS)

main.exe: $(NXDK_DIR)/lib/libSDL2_image.lib

CLEANRULES += clean-libsdlimage

.PHONY: clean-libsdlimage
clean-libsdlimage:
	$(VE)rm -f $(LIBSDLIMAGE_OBJS) $(NXDK_DIR)/lib/libSDL2_image.lib

SDL_GFX_DIR = $(NXDK_DIR)/lib/sdl/SDL2_gfx
SDL_GFX_SRCS += $(SDL_GFX_DIR)/SDL2_framerate.c \
                $(SDL_GFX_DIR)/SDL2_gfxPrimitives.c \
                $(SDL_GFX_DIR)/SDL2_imageFilter.c \
                $(SDL_GFX_DIR)/SDL2_rotozoom.c

SDL_GFX_OBJS = $(addsuffix .obj, $(basename $(SDL_GFX_SRCS)))

$(NXDK_DIR)/lib/libSDL_gfx.lib: $(SDL_GFX_OBJS)

NXDK_CFLAGS += -I$(NXDK_DIR)/lib/sdl \
               -I$(SDL_GFX_DIR) \
               -DXBOX

NXDK_CXXFLAGS += -I$(NXDK_DIR)/lib/sdl \
                 -I$(SDL_GFX_DIR) \
                 -DXBOX

main.exe: $(NXDK_DIR)/lib/libSDL_gfx.lib

CLEANRULES += clean-sdl_gfx

.PHONY: clean-sdl_gfx
clean-sdl_gfx:
	$(VE)rm -f $(SDL_gfx_OBJS) \
             $(NXDK_DIR)/lib/libSDL_gfx.lib

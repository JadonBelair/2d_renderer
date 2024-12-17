#pragma once

#include "../libs/sokol/sokol_gfx.h"
#include "../libs/stb/stb_image.h"
#include "../libs/stb/stb_rect_pack.h"
#include "../libs/stb/stb_image_write.h"

#define NUM_NODES 4096

typedef struct {
	int id;
	int width, height;
} Texture2D;

typedef struct {
	stbi_uc* data;
	stbrp_rect rect;
} ImageData;

Texture2D make_texture(const char*);
ImageData build_atlas();
void destroy_images();
stbrp_rect get_texture_rect(int);

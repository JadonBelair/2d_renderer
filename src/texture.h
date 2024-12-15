#pragma once

#include "../libs/sokol/sokol_gfx.h"

typedef struct {
	sg_image id;
	int width, height;
} Texture2D;

Texture2D make_texture(const char*);
void bind_texture(sg_image*, Texture2D);

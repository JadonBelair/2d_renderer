#define STB_IMAGE_IMPLEMENTATION
#include "../libs/stb_image.h"

#include "texture.h"

Texture2D make_texture(const char* file_path) {
	int width, height, channels;
	const int desired_channels = 4;
	stbi_uc* pixels = stbi_load(file_path, &width, &height, &channels, desired_channels);
	sg_image id = sg_make_image(&(sg_image_desc) {
		.width = width,
		.height = height,
		.pixel_format = SG_PIXELFORMAT_RGBA8,
		.data.subimage[0][0] = {
			.ptr = pixels,
			.size = (size_t)(width*height*4)
		}
	});

	stbi_image_free(pixels);

	Texture2D tex = {
		.id = id,
		.width = width,
		.height = height,
	};

	return tex;
}

void bind_texture(sg_image* slot, Texture2D tex) {
	*slot = tex.id;
}

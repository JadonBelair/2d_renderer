#define STB_IMAGE_IMPLEMENTATION
#define STB_RECT_PACK_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "texture.h"

stbrp_node nodes[NUM_NODES];

stbrp_rect rects[1000] = {0};
stbi_uc* images[1000] = {0};
int count = 0;

Texture2D make_texture(const char* file_path) {
	if (count == 1000) {
		return (Texture2D){
			.id = -1,
		};
	}

	int width, height, channels;
	const int desired_channels = 4;
	stbi_uc* pixels = stbi_load(file_path, &width, &height, &channels, desired_channels);
	
	images[count] = pixels;
	rects[count] = (stbrp_rect) {
		.id = count,
		.w = width,
		.h = height,
	};

	Texture2D tex = {
		.id = count,
		.width = width,
		.height = height,
	};
	count++;

	return tex;
}

ImageData build_atlas() {
	stbrp_context context;
	stbrp_init_target(&context, NUM_NODES, NUM_NODES, nodes, NUM_NODES);

	stbrp_pack_rects(&context, rects, count);

	stbi_uc* image_data = malloc(sizeof(stbi_uc)*(NUM_NODES*NUM_NODES*4));
	memset(image_data, 0, NUM_NODES*NUM_NODES*4);

	for (int i = 0; i < count; i++) {
		stbi_uc* image = images[i];

		for (int y = 0; y < rects[i].h; y++) {
			int atlas_top = rects[i].y + y;
			int atlas_index = atlas_top * (NUM_NODES*4) + (rects[i].x*4);

			int image_index = y * (rects[i].w*4);
			memcpy(&image_data[atlas_index], &image[image_index], rects[i].w*4);
		}
	}

	destroy_images();

	ImageData image = {
		.data = image_data,
		.width = NUM_NODES,
		.height = NUM_NODES,
	};

	return image;
}

void destroy_images() {
	for (int i = 0; i < count; i++) {
		stbi_image_free(images[i]);
	}
}

stbrp_rect get_texture_rect(int id) {
	for (int i = 0; i < count; i++) {
		if (rects[i].id == id) {
			return rects[i];
		}
	}

	return (stbrp_rect) {
		.id = -1,
	};
}

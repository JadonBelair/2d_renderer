#define STB_IMAGE_IMPLEMENTATION
#define STB_RECT_PACK_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "texture.h"
#include "../libs/cglm/vec2.h"

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

void fill_uvs(uv_data_t* uvs) {
	// quad vertex layout
	// 0  1
	// 3  2
	//
	// index order
	// 0, 1, 3
	// 1, 2, 3
	
	for (int i = 0; i < count; i++) {
		int texture_offset = rects[i].id*4;

		vec2 top_left = {(float) rects[i].x / (float) NUM_NODES, };
		vec2 top_right = {(float)(rects[i].x + rects[i].w) / (float)NUM_NODES, (float)rects[i].y / (float)NUM_NODES};
		vec2 bottom_right = {(float)(rects[i].x + rects[i].w) / (float)NUM_NODES, (float)(rects[i].y + rects[i].h) / (float)NUM_NODES};
		vec2 bottom_left = {(float)rects[i].x / (float)NUM_NODES, (float)(rects[i].y + rects[i].h) / (float)NUM_NODES};

		uvs[texture_offset+0].pos[0] = top_left[0];
		uvs[texture_offset+0].pos[1] = top_left[1];

		uvs[texture_offset+1].pos[0] = top_right[0];
		uvs[texture_offset+1].pos[1] = top_right[1];

		uvs[texture_offset+2].pos[0] = bottom_right[0];
		uvs[texture_offset+2].pos[1] = bottom_right[1];

		uvs[texture_offset+3].pos[0] = bottom_left[0];
		uvs[texture_offset+3].pos[1] = bottom_left[1];
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

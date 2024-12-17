#ifdef __linux__
#define SOKOL_GLCORE
#endif

#ifdef _WIN32
#define SOKOL_D3D11
#endif

#ifdef __APPLE__
#define SOKOL_METAL
#endif

#define SOKOL_IMPL
#include "renderer.h"
#include "../shaders/texture-sapp.h"

#define NUM_QUADS 1000
#define NUM_VERTS (NUM_QUADS * 4)

#define NUM_POSITIONS (NUM_QUADS * 16)
#define NUM_POSITIONS_BYTES (NUM_QUADS * (sizeof(float)*16))
#define NUM_UVS NUM_VERTS
#define NUM_UVS_BYTES (NUM_UVS * sizeof(uv_data_t))
float positions[NUM_POSITIONS];
uv_data_t uvs[NUM_UVS];
int instances = 0;

#define pos_offset(i) (i*16)
#define uv_offset(i) (i*4)

#define NUM_KEYS 348
bool is_down[NUM_KEYS] = {false};
bool just_pressed[NUM_KEYS] = {false};
bool just_released[NUM_KEYS] = {false};

float width, height;
window_conf conf;

static struct {
	sg_pipeline pip;
	sg_bindings bind;
	sg_pass_action pass_action;
	mat4 proj;
} state;

static void init(void) {
	stm_setup();

	sg_setup(&(sg_desc) {
		.environment = sglue_environment(),
		.logger.func = slog_func,
	});

	float vertices[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
		1.0f, 1.0f,
        0.0f, 1.0f,
    };

	uint16_t indicies[6] = {
		0, 1, 3,
		1, 2, 3,
	};

	state.bind.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc) {
		.data = SG_RANGE(vertices),
	});

	state.bind.vertex_buffers[1] = sg_make_buffer(&(sg_buffer_desc) {
		.usage = SG_USAGE_STREAM,
		.size = NUM_POSITIONS_BYTES,
	});

	state.bind.vertex_buffers[2] = sg_make_buffer(&(sg_buffer_desc) {
		.usage = SG_USAGE_STREAM,
		.size = NUM_UVS_BYTES,
	});

	state.bind.storage_buffers[0] = sg_make_buffer(&(sg_buffer_desc) {
		.type = SG_BUFFERTYPE_STORAGEBUFFER,
		.usage = SG_USAGE_STREAM,
		.size = NUM_UVS_BYTES,
	});

	state.bind.index_buffer = sg_make_buffer(&(sg_buffer_desc) {
		.type = SG_BUFFERTYPE_INDEXBUFFER,
		.data = SG_RANGE(indicies),
	});

	state.bind.samplers[SMP_smp] = sg_make_sampler(&(sg_sampler_desc) {
		.min_filter = SG_FILTER_LINEAR,
		.mag_filter = SG_FILTER_NEAREST,
	});

	state.pip = sg_make_pipeline(&(sg_pipeline_desc) {
		.shader = sg_make_shader(texture_shader_desc(sg_query_backend())),
		.index_type = SG_INDEXTYPE_UINT16,
		.layout = {
			.attrs = {
				[ATTR_texture_position].format = SG_VERTEXFORMAT_FLOAT2,
				[ATTR_texture_inst_mat0] = {.format = SG_VERTEXFORMAT_FLOAT4, .offset =  0, .buffer_index = 1}, 
				[ATTR_texture_inst_mat1] = {.format = SG_VERTEXFORMAT_FLOAT4, .offset = 16, .buffer_index = 1},
				[ATTR_texture_inst_mat3] = {.format = SG_VERTEXFORMAT_FLOAT4, .offset = 48, .buffer_index = 1},
			},
			.buffers[0] = {.step_func = SG_VERTEXSTEP_PER_VERTEX},
			.buffers[1] = {.step_func = SG_VERTEXSTEP_PER_INSTANCE, .stride = sizeof(float)*16},
		},
		.depth = {
			.compare = SG_COMPAREFUNC_LESS_EQUAL,
			.write_enabled = true,
		},
		.colors[0] = {
			.blend = {
				.enabled = true,
				.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA,
				.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
				.op_rgb = SG_BLENDOP_ADD,
				.src_factor_alpha = SG_BLENDFACTOR_SRC_ALPHA,
				.dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
				.op_alpha = SG_BLENDOP_ADD,
			},
		},
	});

	state.pass_action = (sg_pass_action) {
		.colors[0] = { .load_action=SG_LOADACTION_CLEAR, .clear_value={0.0f, 0.0f, 0.0f, 1.0f} }
	};

	setup();
	ImageData atlas = build_atlas();
	state.bind.images[0] = sg_make_image(&(sg_image_desc) {
		.width = atlas.width,
		.height = atlas.height,
		.pixel_format = SG_PIXELFORMAT_RGBA8,
		.data.subimage[0][0] = {
			.ptr = atlas.data,
			.size = atlas.width * atlas.height * 4,
		},
	});
	stbi_image_free(atlas.data);
}

void frame(void) {
	start_frame();
	update();
	end_frame();
}

void cleanup(void) {
	sg_shutdown();
}

void event(const sapp_event* event) {
	if (event->type == SAPP_EVENTTYPE_KEY_DOWN) {
		if (!is_down[event->key_code]) {
			just_pressed[event->key_code] = true;
		}
		is_down[event->key_code] = true;
	} else if (event->type == SAPP_EVENTTYPE_KEY_UP) {
		just_released[event->key_code] = true;
		is_down[event->key_code] = false;
	}
}

sapp_desc sokol_main(int argc, char **argv) {
	conf = window_config();
	int window_width = conf.window_width != 0 ? conf.window_width : conf.viewport_width;
	int window_height = conf.window_height != 0 ? conf.window_height : conf.viewport_height;
	return (sapp_desc) {
		.init_cb = init,
		.frame_cb = frame,
		.cleanup_cb = cleanup,
		.event_cb = event,
		.width = window_width,
		.height = window_height,
		.window_title = conf.title,
		.icon.sokol_default = true,
		.logger.func = slog_func,
	};
}

void mat_to_1d(mat4 mat, float arr[16]) {
	arr[0]  = mat[0][0]; arr[1]  = mat[0][1]; arr[2]  = mat[0][2]; arr[3]  = mat[0][3];
	arr[4]  = mat[1][0]; arr[5]  = mat[1][1]; arr[6]  = mat[1][2]; arr[7]  = mat[1][3];
	arr[8]  = mat[2][0]; arr[9]  = mat[2][1]; arr[10] = mat[2][2]; arr[11] = mat[2][3];
	arr[12] = mat[3][0]; arr[13] = mat[3][1]; arr[14] = mat[3][2]; arr[15] = mat[3][3];
}

void update_proj() {
	float current_width = sapp_widthf();
	float current_height = sapp_heightf();
	float current_ratio = current_width / current_height;

	float target_ratio = (float)conf.viewport_width / (float)conf.viewport_height;
	if (conf.viewport_width * current_ratio > conf.viewport_width * target_ratio) {
		height = (float)conf.viewport_height;
		width = current_ratio * height;
	} else {
		width = (float)conf.viewport_width;
		height = width / current_ratio;
	}

	glm_ortho(0.0f, width, height, 0.0f, -1.0f, 1.0f, state.proj);
}

void draw_instances() {
	if (instances == 0) return;

	sg_update_buffer(state.bind.vertex_buffers[1], &SG_RANGE(positions));
	sg_update_buffer(state.bind.storage_buffers[0], &SG_RANGE(uvs));
	sg_apply_bindings(&state.bind);

	vs_params_t vs_params;
	mat_to_1d(state.proj, vs_params.p);
	sg_apply_uniforms(UB_vs_params, &SG_RANGE(vs_params));

	sg_draw(0, 6, instances);

	instances = 0;
	memset(positions, 0, sizeof(positions));
	memset(uvs, 0, sizeof(uvs));

	// TODO: try and find a better way of clearing the 
	// buffer besides deleting and making a new one
	sg_destroy_buffer(state.bind.vertex_buffers[1]);
	state.bind.vertex_buffers[1] = sg_make_buffer(&(sg_buffer_desc) {
		.usage = SG_USAGE_STREAM,
		.size = NUM_POSITIONS_BYTES,
	});

	sg_destroy_buffer(state.bind.storage_buffers[0]);
	state.bind.storage_buffers[0] = sg_make_buffer(&(sg_buffer_desc) {
		.type = SG_BUFFERTYPE_STORAGEBUFFER,
		.usage = SG_USAGE_STREAM,
		.size = NUM_UVS_BYTES,
	});
}

void draw_texture(Texture2D tex, vec2 pos, vec2 size, float angle) {
	if (instances == NUM_QUADS) {
		draw_instances();
	}

	// quad vertex layout
	// 0  1
	// 3  2
	//
	// index order
	// 0, 1, 3
	// 1, 2, 3
	

	stbrp_rect texture_rect = get_texture_rect(tex.id);

	int offset = uv_offset(instances);
	uvs[offset+0].pos[0] = (float)texture_rect.x / (float)NUM_NODES;
	uvs[offset+0].pos[1] = (float)texture_rect.y / (float)NUM_NODES;

	uvs[offset+1].pos[0] = (float)(texture_rect.x + tex.width) / (float)NUM_NODES;
	uvs[offset+1].pos[1] = (float)texture_rect.y / (float)NUM_NODES;

	uvs[offset+2].pos[0] = (float)(texture_rect.x + tex.width) / (float)NUM_NODES;
	uvs[offset+2].pos[1] = (float)(texture_rect.y + tex.height) / (float)NUM_NODES;

	uvs[offset+3].pos[0] = (float)texture_rect.x / (float)NUM_NODES;
	uvs[offset+3].pos[1] = (float)(texture_rect.y + tex.height) / (float)NUM_NODES;

	mat4 model;
	glm_mat4_identity(model);

	vec3 pos_3d = {pos[0], pos[1], 0.0f};
	glm_translate(model, pos_3d);

	glm_translate(model, (vec3) {size[0] * 0.5f, size[1] * 0.5f, 0.0f});
	glm_rotate_z(model, angle, model);
	glm_translate(model, (vec3) {size[0] * -0.5f, size[1] * -0.5f, 0.0f});

	vec3 size_3d = {size[0], size[1], 1.0f};
	glm_scale(model, size_3d);

	mat_to_1d(model, &positions[pos_offset(instances)]);
	instances++;
}

uint64_t prev;
uint64_t current;

void start_frame() {
	prev = current;
	current = stm_now();

	update_proj();
	sg_begin_pass(&(sg_pass) { .action=state.pass_action, .swapchain=sglue_swapchain() });
	sg_apply_pipeline(state.pip);
}

void end_frame() {
	draw_instances();

	sg_end_pass();
	sg_commit();

	memset(just_pressed, 0, sizeof(just_pressed));
	memset(just_released, 0, sizeof(just_released));
}

bool is_key_down(sapp_keycode code) {
	return is_down[code];
}

bool is_key_just_pressed(sapp_keycode code) {
	return just_pressed[code];
}

bool is_key_just_released(sapp_keycode code) {
	return just_released[code];
}

float get_view_width() {
	return width;
}

float get_view_height() {
	return height;
}

float delta_time() {
	return stm_sec(stm_diff(current, prev));
}

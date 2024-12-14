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

// target size for viewport,
// define these in your application
extern int target_width, target_height;

#define NUM_KEYS 348
bool is_down[NUM_KEYS] = {false};
bool just_pressed[NUM_KEYS] = {false};
bool just_released[NUM_KEYS] = {false};

float width, height;

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
			}
		},
	});

	state.pass_action = (sg_pass_action) {
		.colors[0] = { .load_action=SG_LOADACTION_CLEAR, .clear_value={0.0f, 0.0f, 0.0f, 1.0f} }
	};

	setup();
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
	return (sapp_desc) {
		.init_cb = init,
		.frame_cb = frame,
		.cleanup_cb = cleanup,
		.event_cb = event,
		.width = target_width,
		.height = target_height,
		.window_title = "Sokol Test App",
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
	if (sapp_width() > sapp_height()) {
		height = (float)target_height;
		width = (sapp_widthf() / sapp_heightf()) * height;
	} else {
		width = (float)target_width;
		height = (sapp_heightf() / sapp_widthf()) * width;
	}

	glm_ortho(0.0f, width, height, 0.0f, -1.0f, 1.0f, state.proj);
}

void draw_texture(Texture2D tex, vec2 pos, vec2 size, float angle) {
	bind_texture(&state.bind.images[IMG_tex], tex);

	mat4 model;
	glm_mat4_identity(model);

	vec3 pos_3d = {pos[0], pos[1], 0.0f};
	glm_translate(model, pos_3d);

	glm_translate(model, (vec3) {size[0] * 0.5f, size[1] * 0.5f, 0.0f});
	glm_rotate_z(model, angle, model);
	glm_translate(model, (vec3) {size[0] * -0.5f, size[1] * -0.5f, 0.0f});

	vec3 size_3d = {size[0], size[1], 1.0f};
	glm_scale(model, size_3d);

	vs_params_t vs_params;
	mat_to_1d(model, vs_params.m);
	mat_to_1d(state.proj, vs_params.p);

	sg_apply_bindings(&state.bind);
	sg_apply_uniforms(UB_vs_params, &SG_RANGE(vs_params));
	sg_draw(0, 6, 1);
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

float get_width() {
	return width;
}

float get_height() {
	return height;
}

float delta_time() {
	return stm_sec(stm_diff(current, prev));
}

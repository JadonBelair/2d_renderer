#include "renderer.h"

int target_width = 600;
int target_height = 400;

vec2 pos;
Texture2D crate;

void setup() {
	pos[0] = 300.0f;
	pos[1] = 350.0f;

	crate = make_texture("image.png");
}

void update() {
	if (is_key_just_pressed(SAPP_KEYCODE_F)) {
		sapp_toggle_fullscreen();
	}
	if (is_key_down(SAPP_KEYCODE_ESCAPE) || is_key_down(SAPP_KEYCODE_Q)) {
		sapp_request_quit();
	}

	vec2 dir = {0.0f, 0.0f};
	if (is_key_down(SAPP_KEYCODE_A) || is_key_down(SAPP_KEYCODE_LEFT)) {
		dir[0] -= 1.0f;
	}
	if (is_key_down(SAPP_KEYCODE_D) || is_key_down(SAPP_KEYCODE_RIGHT)) {
		dir[0] += 1.0f;
	}
	if (is_key_down(SAPP_KEYCODE_W) || is_key_down(SAPP_KEYCODE_UP)) {
		dir[1] -= 1.0f;
	}
	if (is_key_down(SAPP_KEYCODE_S) || is_key_down(SAPP_KEYCODE_DOWN)) {
		dir[1] += 1.0f;
	}
	glm_vec2_normalize(dir);
	pos[0] += dir[0];
	pos[1] += dir[1];

	pos[0] = glm_clamp(pos[0], 0.0f, get_width()-crate.width);
	pos[1] = glm_clamp(pos[1], 0.0f, get_height()-crate.height);

	vec2 size = {crate.width, crate.height};
	draw_texture(crate, pos, size, 0.0f);
}

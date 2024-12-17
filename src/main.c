#include "renderer.h"
#include <stdlib.h>

vec2 pos;
Texture2D crate;
Texture2D button;
Texture2D sword;
float overall;

window_conf window_config() {
	return (window_conf) {
		.title = "Test 2D Renderer",
		.viewport_width = 600,
		.viewport_height = 400,
		.window_width = 1200,
		.window_height = 800,
	};
}

void setup() {
	pos[0] = 300.0f;
	pos[1] = 350.0f;

	overall = 0.0f;

	crate = make_texture("image.png");
	button = make_texture("button.png");
	sword = make_texture("sword.png");

	// printf("%d %d\n", crate.id, button.id);
}

void update() {
	srand(0);
	if (is_key_just_pressed(SAPP_KEYCODE_F)) {
		sapp_toggle_fullscreen();
	}
	if (is_key_down(SAPP_KEYCODE_ESCAPE) || is_key_down(SAPP_KEYCODE_Q)) {
		sapp_request_quit();
	}

	float speed = 300.0f * delta_time();

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
	glm_vec2_scale_as(dir, speed, dir);

	pos[0] += dir[0];
	pos[1] += dir[1];

	pos[0] = glm_clamp(pos[0], 0.0f, get_view_width()-crate.width);
	pos[1] = glm_clamp(pos[1], 0.0f, get_view_height()-crate.height);


	vec2 size = {crate.width, crate.height};
	for (int i = 0; i < 15000; i++) {
		vec2 pos = {rand()%(int)get_view_width(), rand()%(int)get_view_height()};
		Texture2D tex;
		int idx = rand() % 3;
		if (idx == 0) {
			tex = crate;
		} else if (idx == 1) {
			tex = sword;
		} else {
			tex = button;
		}
		draw_texture(tex, pos, size, overall);
	}
	draw_texture(button, pos, size, overall);

	overall += delta_time();
}

#pragma once

#include <math.h>
#include <stdint.h>
#include <string.h>

#include "texture.h"

#include "../libs/sokol/sokol_app.h"
#include "../libs/sokol/sokol_log.h"
#include "../libs/sokol/sokol_glue.h"
#include "../libs/sokol/sokol_time.h"

#include "../libs/cglm/cglm.h"

// holds window config information.
// title: window title.
// target_width: the target width for the viewport.
// target_height: the target height for the viewport.
// window_width: initial width of the window.
// window_height: initial height of the window.
typedef struct {
	char* title;
	int viewport_width;
	int viewport_height;
	int window_width;
	int window_height;
} window_conf;

// define in your program to set basic window and application information
window_conf window_config();

// runs once at the start of the application
void setup();

// runs once each frame
void update();

bool is_key_down(sapp_keycode);
bool is_key_just_pressed(sapp_keycode);
bool is_key_just_released(sapp_keycode);

float get_view_width();
float get_view_height();

float delta_time();

void update_proj();
void mat_to_1d(mat4, float[16]);
void draw_texture(Texture2D, vec2, vec2, float);
void start_frame();
void end_frame();

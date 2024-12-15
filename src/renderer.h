#pragma once

#include <math.h>
#include <stdint.h>
#include <string.h>

#include "texture.h"

#include "../libs/sokol/sokol_app.h"
#include "../libs/sokol/sokol_log.h"
#include "../libs/sokol/sokol_glue.h"
#include "../libs/sokol/sokol_time.h"

#include "../shaders/texture-sapp.h"

#include "../libs/cglm/cglm.h"


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

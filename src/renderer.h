#pragma once

#include <math.h>
#include <stdint.h>
#include <string.h>

#include "includes.h"
#include "texture.h"

// runs once at the start of the application
void setup();

// runs once each frame
void update();

bool is_key_down(sapp_keycode);
bool is_key_just_pressed(sapp_keycode);
bool is_key_just_released(sapp_keycode);

float get_width();
float get_height();

void update_proj();
void mat_to_1d(mat4, float[16]);
void draw_texture(Texture2D, vec2, vec2, float);
void start_frame();
void end_frame();

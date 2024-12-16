@vs vs
in vec2 position;
in vec4 inst_mat0;
in vec4 inst_mat1;
in vec4 inst_mat3;

layout(binding=0) uniform vs_params {
    mat4 p;
};

out vec2 uv;

void main() {
	mat4 m = mat4(inst_mat0, inst_mat1, vec4(0.0,0.0,1.0,1.0), inst_mat3);
    gl_Position = p * m * vec4(position, 0.0, 1.0);
    uv = position;
}
@end

@fs fs
layout(binding=0) uniform texture2D tex;
layout(binding=0) uniform sampler smp;

in vec2 uv;
out vec4 frag_color;

void main() {
    frag_color = texture(sampler2D(tex, smp), uv);
}
@end

@program texture vs fs

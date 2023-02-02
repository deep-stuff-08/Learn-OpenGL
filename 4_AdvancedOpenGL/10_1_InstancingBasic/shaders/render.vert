#version 460 core

layout(location = 0)in vec2 vPos;
layout(location = 1)in vec4 vCol;
layout(location = 2)in vec2 offsets;

out VS_OUT {
	vec4 color;
} vs_out;

void main(void) {
	vs_out.color = vCol;
	gl_Position = vec4(vPos * (gl_InstanceID / 100.0) + offsets, 0.0, 1.0);
}
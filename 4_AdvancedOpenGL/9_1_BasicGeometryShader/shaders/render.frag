#version 460 core

layout(location = 0)out vec4 FragColor;

in GS_OUT {
	vec4 color;
} fs_in;

void main(void) {
	FragColor = fs_in.color;
}
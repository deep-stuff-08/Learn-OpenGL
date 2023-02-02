#version 460 core

in vec2 color;

layout(location = 0)out vec4 FragColor;

void main(void) {
	FragColor = vec4(color, 0.0, 1.0);
}
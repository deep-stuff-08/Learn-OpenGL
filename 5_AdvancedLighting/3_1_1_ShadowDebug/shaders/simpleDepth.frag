#version 460 core

layout(location = 0)out vec4 color;

layout(location = 2)uniform vec4 colors;

void main(void) {
	color = colors;
}
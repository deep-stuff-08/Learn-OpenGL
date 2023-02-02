#version 460 core

in vec2 tex;

layout(binding = 0)uniform sampler2D image;

layout(location = 0)out vec4 FragColor;

void main(void) {
	FragColor = texture(image, tex);
}
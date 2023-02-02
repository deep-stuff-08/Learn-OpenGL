#version 460 core

in vec2 tex;

layout(binding = 0)uniform sampler2D image1;
layout(binding = 1)uniform sampler2D image2;

layout(location = 0)out vec4 FragColor;

void main(void) {
	FragColor = mix(texture(image2, tex), texture(image1, vec2(1.0 - tex.x, tex.y)), 0.2);
}
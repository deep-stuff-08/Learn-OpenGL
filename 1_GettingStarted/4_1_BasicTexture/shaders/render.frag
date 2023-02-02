#version 460 core

layout(location = 0)out vec4 FragColor;
layout(binding = 0)uniform sampler2D texture1;

in vec2 texCoord;

void main(void) {
	FragColor = texture(texture1, texCoord);
}
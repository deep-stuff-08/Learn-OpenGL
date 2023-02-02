#version 460 core

layout(location = 3)uniform vec3 lightColor;
layout(location = 4)uniform vec3 objectColor;

layout(location = 0)out vec4 FragColor;

void main(void) {
	FragColor = vec4(objectColor * lightColor, 1.0);
}
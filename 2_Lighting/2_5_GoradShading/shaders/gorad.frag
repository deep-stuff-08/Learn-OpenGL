#version 460 core

in vec3 lightFinalColor;

layout(location = 4)uniform vec3 objectColor;

layout(location = 0)out vec4 FragColor;

void main(void) {
	FragColor = vec4(lightFinalColor * objectColor, 1.0);
}
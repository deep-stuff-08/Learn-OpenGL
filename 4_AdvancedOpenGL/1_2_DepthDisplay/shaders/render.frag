#version 460 core

in vec2 tex;

layout(binding = 0)uniform sampler2D image;

layout(location = 0)out vec4 FragColor;

layout(location = 3)uniform vec2 nearFar;

float linearizeDepth(float depth) {
	float z = depth * 2.0 - 1.0; 
	return (2.0 * nearFar.x * nearFar.y) / (nearFar.y + nearFar.x - z * (nearFar.y - nearFar.x));
}

void main(void) {
	FragColor = vec4(vec3(linearizeDepth(gl_FragCoord.z) / nearFar.y), 1.0);
}
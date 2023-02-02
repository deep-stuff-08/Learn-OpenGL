#version 460 core

layout(location = 0)out vec4 FragColor;

in VS_OUT {
	vec2 texCoords;
} fs_in;

layout(binding = 0)uniform sampler2D scene;
layout(binding = 1)uniform sampler2D bloomBlur;
layout(location = 0)uniform bool bloom;
layout(location = 1)uniform float exposure;

void main() {
	const float gamma = 2.2;
	vec3 hdrColor = texture(scene, fs_in.texCoords).rgb;      
	vec3 bloomColor = texture(bloomBlur, fs_in.texCoords).rgb;
	if(bloom) {
		hdrColor += bloomColor;
	}
	vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
	result = pow(result, vec3(1.0 / gamma));
	FragColor = vec4(result, 1.0);
}
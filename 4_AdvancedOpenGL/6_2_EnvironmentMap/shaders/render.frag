#version 460 core

in vec3 pos;
in vec3 nor;

layout(binding = 0)uniform samplerCube image;

layout(location = 3)uniform vec3 cameraPos;
layout(location = 5)uniform bool isReflect;

layout(location = 0)out vec4 FragColor;

void main(void) {
	vec3 I = vec3(pos - cameraPos);
	vec3 R;
	if(isReflect) {
		R = reflect(I, normalize(nor));
	} else {
		R = refract(I, normalize(nor), 1.0 / 1.5);
	}
	FragColor = texture(image, vec3(R.x, -R.y, R.z));
}
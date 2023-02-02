#version 460 core

in vec4 FragPos;

layout(location = 7)uniform vec3 lightPos;
layout(location = 8)uniform float farPlane;

void main() {
	float lightDist = length(FragPos.xyz - lightPos);
	lightDist = lightDist / farPlane;
	gl_FragDepth = lightDist;
}
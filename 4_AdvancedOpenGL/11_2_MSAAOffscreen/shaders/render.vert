#version 460 core

layout(location = 0)in vec3 vPos;

layout(location = 0)uniform mat4 pMatrix;
layout(location = 1)uniform mat4 vMatrix;
layout(location = 2)uniform mat4 mMatrix;

void main(void) {
	gl_Position = pMatrix * vMatrix * mMatrix * vec4(vPos, 1.0);
}
#version 460 core

layout(location = 0)in vec2 vPos;

layout(location = 0)uniform mat4 mvpMatrix;

void main(void) {
	gl_Position = mvpMatrix * vec4(vPos, 0.0, 1.0);
}
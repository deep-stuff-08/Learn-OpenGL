#version 460 core

layout(location = 0)in vec3 vPos;
layout(location = 1)in vec3 vCol;

layout(location = 0)uniform mat4 mvpMatrix;

out vec4 color;

void main(void) {
	gl_Position = mvpMatrix * vec4(vPos, 1.0);
	color = vec4(vCol, 1.0);
}
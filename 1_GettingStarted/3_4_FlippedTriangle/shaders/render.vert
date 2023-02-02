#version 460 core

layout(location = 0)in vec2 vPos;

layout(location = 0)uniform mat4 mvpMatrix;

void main(void) {
	vec2 vNewPos = vec2(vPos.x, -vPos.y);
	gl_Position = mvpMatrix * vec4(vNewPos, 0.0, 1.0);
}
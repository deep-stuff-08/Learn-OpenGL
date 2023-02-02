#version 460 core

layout(location = 0)in vec4 aPos;
layout(location = 1)in vec3 aNor;
layout(location = 2)in vec2 aTex;

layout(location = 0)uniform mat4 mMat;

void main() {
	gl_Position = mMat * aPos;
}
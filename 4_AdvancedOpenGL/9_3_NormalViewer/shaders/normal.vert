#version 460 core

layout(location = 0)in vec4 vPos;
layout(location = 1)in vec3 vNor;
layout(location = 2)in vec2 vTex;

layout(location = 1)uniform mat4 vMat;
layout(location = 2)uniform mat4 mMat;

out vec3 normals;

void main(void) {
	mat4 modelView = vMat * mMat;
	normals = mat3(modelView) * vNor;
	gl_Position = modelView * vPos;
}
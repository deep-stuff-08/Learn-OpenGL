#version 460 core

layout(location = 0)in vec4 vPos;
layout(location = 1)in vec3 vNor;
layout(location = 2)in vec2 vTex;
layout(location = 3)in mat4 instanceModel;

layout(location = 0)uniform mat4 pMat;
layout(location = 1)uniform mat4 vMat;
layout(location = 2)uniform mat4 mMat;

out VS_OUT {
	vec3 N;
	vec3 L;
	vec3 V;
	vec2 Tex;
} vs_out;

void main(void) {
	mat4 mvMat = vMat * mMat * instanceModel;
	vec4 P = mvMat * vPos;
	gl_Position = pMat * P;
	vs_out.N = mat3(mvMat) * vNor;
	vs_out.L = vec3(1000.0, 1000.0, 1000.0) - P.xyz;
	vs_out.V = -P.xyz;
	vs_out.Tex = vTex;
}
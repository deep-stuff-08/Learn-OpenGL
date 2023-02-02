#version 460 core

layout(location = 0)in vec3 vPos;
layout(location = 1)in vec3 vNor;
layout(location = 2)in vec2 vTex;

layout(location = 0)uniform mat4 pMatrix;
layout(location = 1)uniform mat4 vMatrix;
layout(location = 2)uniform mat4 mMatrix;

out VS_OUT {
	vec3 fragPos;
	vec3 normal;
	vec2 tex;
} vs_out;

void main(void) {
	vs_out.tex = vTex;
	vs_out.normal = mat3(mMatrix) * vNor;
	vs_out.fragPos = (mMatrix * vec4(vPos, 1.0)).xyz;
	gl_Position = pMatrix * vMatrix * mMatrix * vec4(vPos, 1.0);
}
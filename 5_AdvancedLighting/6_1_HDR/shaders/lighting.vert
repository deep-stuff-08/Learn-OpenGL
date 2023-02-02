#version 460 core

layout(location = 0)in vec4 vPos;
layout(location = 1)in vec3 vNor;
layout(location = 2)in vec2 vTex;

out VS_OUT {
	vec3 fragPos;
	vec3 normal;
	vec2 texCoords;
} vs_out;

layout(location = 0)uniform mat4 pMat;
layout(location = 1)uniform mat4 vMat;
layout(location = 2)uniform mat4 mMat;

layout(location = 3)uniform bool invertNormal;

void main(void) {
	vec4 p = mMat * vPos;
	vs_out.fragPos = p.xyz;
	vs_out.texCoords = vTex;
	
	vec3 n = invertNormal ? -vNor : vNor;
	vs_out.normal = normalize(mat3(mMat) * n);

	gl_Position = pMat * vMat * p;
	//gl_Position = vPos;
}
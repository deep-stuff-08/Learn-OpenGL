#version 460 core

layout(location = 0)in vec3 pos;
layout(location = 1)in vec3 norm;
layout(location = 2)in vec2 tex;
layout(location = 3)in ivec4 boneIds; 
layout(location = 4)in vec4 weights;

layout(location = 0)uniform mat4 projection;
layout(location = 1)uniform mat4 view;
layout(location = 2)uniform mat4 model;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
layout(location = 3)uniform mat4 finalBonesMatrices[MAX_BONES];

out vec3 positions;
out vec3 normals;
out vec2 texCoords;

void main() {
	vec4 totalPosition = vec4(0.0);
	vec3 totalNormal = vec3(0.0);
	for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++) {
		if(boneIds[i] == -1) {
			continue;
		}
		vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(pos,1.0);
		totalPosition += localPosition * weights[i];
		vec3 localNormal = mat3(finalBonesMatrices[boneIds[i]]) * norm;
		totalNormal += localNormal;
	}
	
	mat4 viewModel = view * model;
	gl_Position =  projection * viewModel * totalPosition;
	positions = (model * totalPosition).xyz;
	normals = normalize(mat3(model) * totalNormal);
	texCoords = tex;
}

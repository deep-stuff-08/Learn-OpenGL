#version 460 core
out vec4 FragColor;

layout(binding = 0)uniform sampler2D texture_diffuse;
layout(binding = 1)uniform sampler2D texture_specular;

void calcPhongLightInWorld(in vec3 posInWorld, in vec3 norInWorld, in vec3 lightPos, in vec3 viewPos, in float shininess, out float diffuse, out float specular);
void calcBlinnPhongLightInWorld(in vec3 posInWorld, in vec3 norInWorld, in vec3 lightPos, in vec3 viewPos, in float shininess, out float diffuse, out float specular);
void calcPhongLightInEye(in vec3 posInEye, in vec3 norInEye, in vec3 lightPos, in float shininess, out float diffuse, out float specular);
void calcBlinnPhongLightInEye(in vec3 posInEye, in vec3 norInEye, in vec3 lightPos, in float shininess, out float diffuse, out float specular);

in vec3 positions;
in vec3 normals;
in vec2 texCoords;

layout(location = 104)uniform vec3 viewPos;

void main() {
	float diffuse, specular;
	calcBlinnPhongLightInWorld(positions, normals, vec3(100.0, 100.0, 100.0), viewPos, 45.0, diffuse, specular);
	FragColor = texture(texture_diffuse, texCoords) * diffuse + texture(texture_specular, texCoords) * vec4(vec3(0.67) * specular, 1.0);
}

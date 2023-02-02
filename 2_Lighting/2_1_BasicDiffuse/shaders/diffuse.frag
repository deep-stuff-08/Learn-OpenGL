#version 460 core

in vec3 normal;
in vec3 fragPos;

layout(location = 3)uniform vec3 lightColor;
layout(location = 4)uniform vec3 objectColor;
layout(location = 5)uniform vec3 lightPos;

layout(location = 0)out vec4 FragColor;

void main(void) {
	vec3 N = normalize(normal);
	vec3 L = normalize(lightPos - fragPos);

	float ambientStrenght = 0.1;
	float diffuseStrenght = max(dot(N, L), 0.0);

	vec3 ambient = lightColor * ambientStrenght;
	vec3 diffuse = lightColor * diffuseStrenght;

	FragColor = vec4(objectColor * (ambient + diffuse), 1.0);
}
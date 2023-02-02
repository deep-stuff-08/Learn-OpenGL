#version 460 core

in vec3 normal;
in vec3 fragPos;
in vec3 lightPos;

layout(location = 3)uniform vec3 lightColor;
layout(location = 4)uniform vec3 objectColor;

layout(location = 0)out vec4 FragColor;

void main(void) {
	vec3 N = normalize(normal);
	vec3 L = normalize(lightPos - fragPos);

	vec3 V = normalize(-fragPos);
	vec3 R = reflect(-L, N);

	float ambientStrenght = 0.1;
	float diffuseStrenght = max(dot(N, L), 0.0);
	float specularStrenght = pow(max(dot(V, R), 0.0), 32);

	vec3 ambient = lightColor * ambientStrenght;
	vec3 diffuse = lightColor * diffuseStrenght;
	vec3 specular = lightColor * specularStrenght;

	FragColor = vec4(objectColor * (ambient + diffuse + specular), 1.0);
}
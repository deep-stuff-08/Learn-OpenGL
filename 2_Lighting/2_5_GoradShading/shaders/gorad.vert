#version 460 core

layout(location = 0)in vec3 vPos;
layout(location = 1)in vec3 vNor;

layout(location = 0)uniform mat4 pMatrix;
layout(location = 1)uniform mat4 vMatrix;
layout(location = 2)uniform mat4 mMatrix;
layout(location = 3)uniform vec3 lightColor;
layout(location = 5)uniform vec3 lightPos;
layout(location = 6)uniform vec3 viewPos;

out vec3 lightFinalColor;

void main(void) {
	vec3 normal = mat3(mMatrix) * vNor;
	vec3 fragPos = vec3(mMatrix * vec4(vPos, 1.0));

	vec3 N = normalize(normal);
	vec3 L = normalize(lightPos - fragPos);

	vec3 V = normalize(viewPos - fragPos);
	vec3 R = reflect(-L, N);

	float ambientStrenght = 0.1;
	float diffuseStrenght = max(dot(N, L), 0.0);
	float specularStrenght = pow(max(dot(V, R), 0.0), 32);

	vec3 ambient = lightColor * ambientStrenght;
	vec3 diffuse = lightColor * diffuseStrenght;
	vec3 specular = lightColor * specularStrenght;

	lightFinalColor = ambient + diffuse + specular;

	gl_Position = pMatrix * vMatrix * mMatrix * vec4(vPos, 1.0);
}
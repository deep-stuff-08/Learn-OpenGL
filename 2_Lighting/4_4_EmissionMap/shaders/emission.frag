#version 460 core

in vec3 normal;
in vec3 fragPos;
in vec2 texcoord;

layout(location = 3)uniform vec3 viewPos;
layout(location = 4)uniform vec3 lightPos;
layout(location = 5)uniform vec3 lightAmbientColor;
layout(location = 6)uniform vec3 lightDiffuseColor;
layout(location = 7)uniform vec3 lightSpecularColor;
layout(location = 8)uniform vec3 materialAmbientColor;
layout(location = 11)uniform float materialShininess;

layout(binding = 0)uniform sampler2D materialDiffuseTexture;
layout(binding = 1)uniform sampler2D materialSpecularTexture;
layout(binding = 2)uniform sampler2D materialEmissionTexture;

layout(location = 0)out vec4 FragColor;

void main(void) {
	vec3 N = normalize(normal);
	vec3 L = normalize(lightPos - fragPos);

	vec3 V = normalize(viewPos - fragPos);
	vec3 R = reflect(-L, N);

	float ambientStrenght = 0.1;
	float diffuseStrenght = max(dot(N, L), 0.0);
	float specularStrenght = pow(max(dot(V, R), 0.0), materialShininess);

	vec3 ambient = lightAmbientColor * materialAmbientColor * ambientStrenght;
	vec3 diffuse = lightDiffuseColor * texture(materialDiffuseTexture, texcoord).rgb * diffuseStrenght;
	vec3 specular = lightSpecularColor * texture(materialSpecularTexture, texcoord).rgb * specularStrenght;
	vec3 emission = texture(materialEmissionTexture, texcoord).rgb;

	FragColor = vec4(ambient + diffuse + specular + emission, 1.0);
}
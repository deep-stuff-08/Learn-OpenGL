#version 460 core 

layout(location = 0)out vec4 FragColor;

in VS_OUT {
	vec3 fragPos;
	vec3 normal;
	vec2 texCoords;
} fs_in;

layout(binding = 0)uniform sampler2D diffuse;
layout(binding = 1)uniform samplerCube depthMap;

layout(location = 4)uniform vec3 lightPos;
layout(location = 5)uniform vec3 viewPos;
layout(location = 6)uniform float farPlane;

float shadowCalculation(vec3 fragPos) {
	vec3 fragToLight = fragPos - lightPos;
	float closestDepth = texture(depthMap, fragToLight).r;
	closestDepth *= farPlane;
	float currentDepth = length(fragToLight);
	float bias = 0.05;
	float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
	return shadow;
}

void main(void) {
	vec3 color = texture(diffuse, fs_in.texCoords).rgb;
	vec3 normal = normalize(fs_in.normal);
	vec3 lightColor = vec3(0.3);
	vec3 ambient = 0.3 * lightColor;
	vec3 lightDir = normalize(lightPos - fs_in.fragPos);
	vec3 diffuse = lightColor * max(dot(lightDir, normal), 0.0);
	vec3 viewDir = normalize(viewPos - fs_in.fragPos);
	vec3 halfDir = normalize(lightDir + viewDir);
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 specular = pow(max(dot(viewDir, reflectDir), 0.0), 64.0) * lightColor;
	float shadow = shadowCalculation(fs_in.fragPos);
	// float shadow = 0.0;
	vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
	FragColor = vec4(lighting, 1.0);
}
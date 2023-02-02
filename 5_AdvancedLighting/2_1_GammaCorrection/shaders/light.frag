#version 460 core 

layout(location = 0)out vec4 FragColor;

in VS_OUT {
	vec3 fragPos;
	vec3 normal;
	vec2 texCoords;
} fs_in;

layout(binding = 0)uniform sampler2D diffuse;

layout(location = 3)uniform vec3 viewPos;
layout(location = 4)uniform bool gamma;
layout(location = 5)uniform vec3 lightPos[4];
layout(location = 9)uniform vec3 lightColor[4];

vec3 blinnPhong(vec3 normal, vec3 fragPos, int i) {
	vec3 lightDir = normalize(lightPos[i] - fragPos);
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = diff * lightColor[i];
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = 0.0;
	vec3 halfwayDir = normalize(lightDir + viewDir);  
	spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
	vec3 specular = spec * lightColor[i];    
	float max_distance = 1.5;
	float distance = length(lightPos[i] - fragPos);
	float attenuation = 1.0 / (gamma ? distance * distance : distance);
	diffuse *= attenuation;
	specular *= attenuation;
	return diffuse + specular;
}

void main(void) {
	vec3 color = texture(diffuse, fs_in.texCoords).rgb;
	vec3 lighting = vec3(0.0);
	for(int i = 0; i < 4; ++i) {
		lighting += blinnPhong(normalize(fs_in.normal), fs_in.fragPos, i);
	}
	color *= lighting;
	if(gamma) {
		color = pow(color, vec3(1.0/2.2));
	}
	FragColor = vec4(color, 1.0);
}
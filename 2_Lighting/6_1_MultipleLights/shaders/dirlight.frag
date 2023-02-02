#version 460 core

in VS_OUT {
	vec3 fragPos;
	vec3 normal;
	vec2 tex;
} fs_in;

layout(location = 3)uniform vec3 viewPos;
layout(location = 4)uniform float matShininess;

layout(binding = 0)uniform sampler2D diffuseTex;
layout(binding = 1)uniform sampler2D specularTex;

struct DirectionalLight {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct SpotLight {
	vec3 position;
	float innerCutoff;
	vec3 direction;
	float outerCutoff;
	vec3 ambient;
	float constant;
	vec3 diffuse;
	float linear;
	vec3 specular;
	float quadratic;
};

struct PointLight {
	vec3 position;
	vec3 ambient;
	float constant;
	vec3 diffuse;
	float linear;
	vec3 specular;
	float quadratic;
};

layout(binding = 0,std140) uniform lightUBO {
	DirectionalLight direction;
	SpotLight spot;
	PointLight point[4];
} light;

layout(location = 0)out vec4 FragColor;

vec3 calcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir) {
	vec3 ambient = light.ambient * texture(diffuseTex, fs_in.tex).rgb;
	vec3 lightDir = normalize(-light.direction);  
	vec3 reflectDir = reflect(-lightDir, normal);  
	vec3 diffuse = light.diffuse * max(dot(normal, lightDir), 0.0) * texture(diffuseTex, fs_in.tex).rgb; 
	vec3 specular = light.specular * pow(max(dot(viewDir, reflectDir), 0.0), matShininess) * texture(specularTex, fs_in.tex).rgb;  
	vec3 result = ambient + diffuse + specular;
	return result;
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 viewDir) {
	vec3 lightDir = normalize(light.position - fs_in.fragPos);  
	float theta = dot(lightDir, normalize(-light.direction));
	float innerCutOff = cos(radians(light.innerCutoff));
	float outerCutOff = cos(radians(light.outerCutoff));
	float intensity = clamp((theta - outerCutOff) / (innerCutOff - outerCutOff), 0.0, 1.0);
	
	vec3 ambient = light.ambient * texture(diffuseTex, fs_in.tex).rgb;
	vec3 reflectDir = reflect(-lightDir, normal);  
	float dist = distance(light.position, fs_in.fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));    
	
	vec3 diffuse = intensity * attenuation * light.diffuse * max(dot(normal, lightDir), 0.0) * texture(diffuseTex, fs_in.tex).rgb; 
	vec3 specular = intensity * attenuation * light.specular * pow(max(dot(viewDir, reflectDir), 0.0), matShininess) * texture(specularTex, fs_in.tex).rgb;  
	
	vec3 result = ambient + diffuse + specular;
	return result;
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir) {
	vec3 lightDir = normalize(light.position - fs_in.fragPos);  
	vec3 reflectDir = reflect(-lightDir, normal);  
	float dist = distance(light.position, fs_in.fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));    
	vec3 ambient = attenuation * light.ambient * texture(diffuseTex, fs_in.tex).rgb;
	vec3 diffuse = attenuation * light.diffuse * max(dot(normal, lightDir), 0.0) * texture(diffuseTex, fs_in.tex).rgb; 
	vec3 specular = attenuation * light.specular * pow(max(dot(viewDir, reflectDir), 0.0), matShininess) * texture(specularTex, fs_in.tex).rgb;  
	vec3 result = ambient + diffuse + specular;
	return result;
}

void main(void) {
	vec3 norm = normalize(fs_in.normal);
	vec3 viewDir = normalize(viewPos - fs_in.fragPos);
	vec3 result = calcDirectionalLight(light.direction, norm, viewDir);
	for(int i = 0; i < 4; i++) {
		result += calcPointLight(light.point[i], norm, viewDir);
	}
	result += calcSpotLight(light.spot, norm, viewDir);
	FragColor = vec4(result, 1.0);
}
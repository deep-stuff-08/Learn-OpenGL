#version 460 core

in VS_OUT {
	vec3 fragPos;
	vec3 normal;
	vec2 tex;
} fs_in;

layout(location = 3)uniform vec3 viewPos;
layout(location = 4)uniform vec3 lightPosition;
layout(location = 5)uniform vec3 lightDirection;
layout(location = 6)uniform float lightCutOff;
layout(location = 7)uniform vec3 lightAmbient;
layout(location = 8)uniform vec3 lightDiffuse;
layout(location = 9)uniform vec3 lightSpecular;
layout(location = 10)uniform vec3 lightAttenuation;
layout(location = 11)uniform float matShininess;

layout(binding = 0)uniform sampler2D diffuseTex;
layout(binding = 1)uniform sampler2D specularTex;

layout(location = 0)out vec4 FragColor;

void main(void) {
	vec3 lightDir = normalize(lightPosition - fs_in.fragPos);  
	float theta = dot(lightDir, normalize(-lightDirection));
	vec3 ambient = lightAmbient * texture(diffuseTex, fs_in.tex).rgb;
	if(theta > cos(radians(lightCutOff))) {
		vec3 norm = normalize(fs_in.normal);
		vec3 viewDir = normalize(viewPos - fs_in.fragPos);
		vec3 reflectDir = reflect(-lightDir, norm);  
		float dist = distance(lightPosition, fs_in.fragPos);
		float attenuation = 1.0 / (lightAttenuation.x + lightAttenuation.y * dist + lightAttenuation.z * (dist * dist));    
		
		vec3 diffuse = attenuation * lightDiffuse * max(dot(norm, lightDir), 0.0) * texture(diffuseTex, fs_in.tex).rgb; 
		vec3 specular = attenuation * lightSpecular * pow(max(dot(viewDir, reflectDir), 0.0), matShininess) * texture(specularTex, fs_in.tex).rgb;  
		
		vec3 result = ambient + diffuse + specular;
		FragColor = vec4(result, 1.0);
	} else {
		FragColor = vec4(ambient, 1.0);	
	}
}
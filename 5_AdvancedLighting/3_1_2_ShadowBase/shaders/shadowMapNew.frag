#version 460 core 

layout(location = 0)out vec4 FragColor;

in VS_OUT {
	vec3 fragPos;
	vec3 normal;
	vec2 texCoords;
	vec4 fragPosLightSpace;
} fs_in;

layout(binding = 0)uniform sampler2D diffuse;
layout(binding = 1)uniform sampler2DShadow shadowmap;

layout(location = 4)uniform vec3 lightPos;
layout(location = 5)uniform vec3 viewPos;

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
	float shadow = textureProj(shadowmap, fs_in.fragPosLightSpace);;
	vec3 lighting = (ambient + shadow * (diffuse + specular)) * color;
	FragColor = vec4(lighting, 1.0);
}
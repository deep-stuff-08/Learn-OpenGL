#version 460 core 

layout(location = 0)out vec4 FragColor;

in VS_OUT {
	vec3 fragPos;
	vec3 normal;
	vec2 texCoords;
} fs_in;

layout(binding = 0)uniform sampler2D diffuse;

layout(location = 3)uniform vec3 lightPos;
layout(location = 4)uniform vec3 viewPos;
layout(location = 5)uniform bool blinn;

void main(void) {
	vec3 color = texture(diffuse, fs_in.texCoords).rgb;
	vec3 ambient = 0.05 * color;
	vec3 lightDir = normalize(lightPos - fs_in.fragPos);
	vec3 normal = normalize(fs_in.normal);
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = diff * color;
	vec3 viewDir = normalize(viewPos - fs_in.fragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = 0.0;
	if(blinn) {
		vec3 halfwayDir = normalize(lightDir + viewDir);  
		spec = pow(max(dot(normal, halfwayDir), 0.0), 128.0);
	} else {
		vec3 reflectDir = reflect(-lightDir, normal);
		spec = pow(max(dot(viewDir, reflectDir), 0.0), 128.0);
	}
	vec3 specular = vec3(0.3) * spec;
	FragColor = vec4(ambient + diffuse + specular, 1.0);
}
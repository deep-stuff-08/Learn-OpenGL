#version 460 core

layout(location = 0)out vec4 FragColor;

layout(binding = 0)uniform sampler2D textureDiffuse;
layout(binding = 1)uniform sampler2D textureNormal;
layout(binding = 2)uniform sampler2D textureDisplacement;

in VS_OUT {
	vec3 fragPos;
	vec2 texCoord;
	vec3 tangentLightPos;
	vec3 tangentViewPos;
	vec3 tangentFragPos;
} fs_in;

layout(location = 5)uniform float heightScale = 0.1;

void main(void) {
	vec3 viewDir = normalize(fs_in.tangentViewPos - fs_in.tangentFragPos);	
	float height =  texture(textureDisplacement, fs_in.texCoord).r;
	const float minLayers = 8;
	const float maxLayers = 32;
	float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  
	float layerDepth = 1.0 / numLayers;
	float currentLayerDepth = 0.0;
	vec2 P = viewDir.xy / viewDir.z * heightScale; 
	vec2 deltaTexCoords = P / numLayers;
	vec2 currentTexCoords = fs_in.texCoord;
	float currentDepthMapValue = texture(textureDisplacement, currentTexCoords).r;
	while(currentLayerDepth < currentDepthMapValue) {
		currentTexCoords -= deltaTexCoords;
		currentDepthMapValue = texture(textureDisplacement, currentTexCoords).r;  
		currentLayerDepth += layerDepth;  
	}
	vec2 texCoord = currentTexCoords;
	vec3 normal = texture(textureNormal, texCoord).rgb;
	normal = normalize(normal * 2.0 - 1.0);   
	vec3 color = texture(textureDiffuse, texCoord).rgb;
	vec3 ambient = 0.1 * color;
	vec3 lightDir = normalize(fs_in.tangentLightPos - fs_in.tangentFragPos);
	vec3 diffuse = max(dot(lightDir, normal), 0.0) * color;
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 halfwayDir = normalize(lightDir + viewDir);  
	vec3 specular = vec3(0.2) * pow(max(dot(normal, halfwayDir), 0.0), 132.0);
	FragColor = vec4(ambient + diffuse + specular, 1.0);
}
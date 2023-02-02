#version 460 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in VS_OUT {
    vec3 fragPos;
    vec3 normal;
    vec2 texCoords;
} fs_in;

layout(location = 3)uniform vec3 viewPos;
layout(location = 4)uniform vec4 lightsPos[4];
layout(location = 9)uniform vec4 lightsColor[4];
layout(location = 13)uniform int lightIndex;

layout(binding = 0)uniform sampler2D diffuseTexture;

void main(void) {
	vec3 result;
	if(lightIndex != -1) {
		result = lightsColor[lightIndex].rgb;
	} else {
		vec3 color = texture(diffuseTexture, fs_in.texCoords).rgb;
		vec3 normal = normalize(fs_in.normal);
		vec3 ambient = 0.0 * color;
		vec3 lighting = vec3(0.0);
		vec3 viewDir = normalize(viewPos - fs_in.fragPos);
		for(int i = 0; i < 4; i++) {
			vec3 lightDir = normalize(lightsPos[i].xyz - fs_in.fragPos);
			vec3 result = lightsColor[i].rgb * max(dot(lightDir, normal), 0.0) * color;
			float dist = distance(fs_in.fragPos, lightsPos[i].xyz);
			result *= 1.0 / (dist * dist);
			lighting += result;
		}
		result = ambient + lighting;
	}
	float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
	if(brightness > 1.0) {
		BrightColor = vec4(result, 1.0);
	} else {
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
	FragColor = vec4(result, 1.0);
}
#version 460 core

in VS_OUT {
	vec3 fragPos;
	vec3 normal;
	vec2 texCoords;
} fs_in;

struct light_t {
	vec4 position;
	vec4 color;
};

layout(location = 4)uniform vec3 viewPos;
layout(location = 5)uniform light_t lights[4];
layout(binding = 0)uniform sampler2D woodTexture;

layout(location = 0)out vec4 FragColor;

void main(void) {
	vec3 color = texture(woodTexture, fs_in.texCoords).rgb;
	vec3 normal = normalize(fs_in.normal);
	vec3 ambient = 0.0 * color;
	vec3 lighting = vec3(0.0);
	for(int i = 0; i < 4; i++) {
		vec3 lightDir = normalize(lights[i].position.rgb - fs_in.fragPos);
		vec3 diffuse = color * lights[i].color.rgb * max(dot(lightDir, normal), 0.0);
		vec3 result = diffuse;

		float dist = length(fs_in.fragPos - lights[i].position.rgb);
		result *= 1.0 / (dist * dist);
		lighting += result;
	}
	FragColor = vec4(ambient + lighting, 1.0);
}
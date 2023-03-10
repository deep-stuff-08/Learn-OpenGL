#version 460 core

layout(location = 0)out vec4 FragColor;

in VS_OUT {
	vec2 texCoords;
} fs_in;

layout(binding = 0)uniform sampler2D image;

layout(location = 0)uniform bool horizontal;
uniform float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

void main(void) {
	vec2 tex_offset = 1.0 / textureSize(image, 0);
	vec3 result = texture(image, fs_in.texCoords).rgb * weight[0];
	if(horizontal) {
		for(int i = 1; i < 5; ++i) {
			result += texture(image, fs_in.texCoords + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
			result += texture(image, fs_in.texCoords - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
		}
	}
	else {
		for(int i = 1; i < 5; ++i) {
			result += texture(image, fs_in.texCoords + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
			result += texture(image, fs_in.texCoords - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
		}
	}
	FragColor = vec4(result, 1.0);
}
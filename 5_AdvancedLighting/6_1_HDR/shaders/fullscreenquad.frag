#version 460 core

layout(location = 0) out vec4 FragColor;

layout(binding = 0) uniform sampler2D texsam;
layout(location = 0) uniform bool hdr;
layout(location = 1) uniform float exposure;

in VS_OUT {
	vec2 texCoords;
} fs_in;

void main(void) {
	const float gamma = 2.2;
	vec3 hdrColor = texture(texsam, fs_in.texCoords).rgb;
	if(hdr)
	{
		vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
		result = pow(result, vec3(1.0 / gamma));
		FragColor = vec4(result, 1.0);
	}
	else
	{
		vec3 result = pow(hdrColor, vec3(1.0 / gamma));
		FragColor = vec4(result, 1.0);
	}
}
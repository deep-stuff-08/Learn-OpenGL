#version 460 core

in VS_OUT {
	vec2 texCoords;
} fs_in;

layout(binding = 0)uniform sampler2D image;

layout(location = 0)out vec4 FragColor;

layout(location = 0)uniform int currentScene;

void main(void) {
	if(currentScene == 0) {
		vec3 color = texture(image, fs_in.texCoords).rgb;
		FragColor = vec4(1.0 - color, 1.0);
	} else if(currentScene == 1) {
		vec3 color = texture(image, fs_in.texCoords).rgb;
		float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
		FragColor = vec4(average, average, average, 1.0);
	} else if(currentScene == 2) {
		const float offset = 1.0 / 300.0;
		vec2 offsets[9] = vec2[](
			vec2(-offset,  offset), // top-left
			vec2( 0.0f,    offset), // top-center
			vec2( offset,  offset), // top-right
			vec2(-offset,  0.0f),   // center-left
			vec2( 0.0f,    0.0f),   // center-center
			vec2( offset,  0.0f),   // center-right
			vec2(-offset, -offset), // bottom-left
			vec2( 0.0f,   -offset), // bottom-center
			vec2( offset, -offset)  // bottom-right    
		);

		float kernel[9] = float[](
			-1, -1, -1,
			-1,  9, -1,
			-1, -1, -1
		);
		
		vec3 sampleTex[9];
		for(int i = 0; i < 9; i++)
		{
			sampleTex[i] = vec3(texture(image, fs_in.texCoords + offsets[i]));
		}
		vec3 col = vec3(0.0);
		for(int i = 0; i < 9; i++)
			col += sampleTex[i] * kernel[i];
		FragColor = vec4(col, 1.0);
	} else if(currentScene == 3) {
		const float offset = 1.0 / 500.0;
		vec2 offsets[9] = vec2[](
			vec2(-offset,  offset), // top-left
			vec2( 0.0f,    offset), // top-center
			vec2( offset,  offset), // top-right
			vec2(-offset,  0.0f),   // center-left
			vec2( 0.0f,    0.0f),   // center-center
			vec2( offset,  0.0f),   // center-right
			vec2(-offset, -offset), // bottom-left
			vec2( 0.0f,   -offset), // bottom-center
			vec2( offset, -offset)  // bottom-right    
		);

		float kernel[9] = float[](
			1, 1, 1,
			1, -8,1,
			1, 1, 1
		);
		
		vec3 sampleTex[9];
		for(int i = 0; i < 9; i++)
		{
			sampleTex[i] = vec3(texture(image, fs_in.texCoords + offsets[i]));
		}
		vec3 col = vec3(0.0);
		for(int i = 0; i < 9; i++)
			col += sampleTex[i] * kernel[i];
		
		FragColor = vec4(col, 1.0);
	} else if(currentScene == 4) {
		const float offset = 1.0 / 500.0;
		vec2 offsets[9] = vec2[](
			vec2(-offset,  offset), // top-left
			vec2( 0.0f,    offset), // top-center
			vec2( offset,  offset), // top-right
			vec2(-offset,  0.0f),   // center-left
			vec2( 0.0f,    0.0f),   // center-center
			vec2( offset,  0.0f),   // center-right
			vec2(-offset, -offset), // bottom-left
			vec2( 0.0f,   -offset), // bottom-center
			vec2( offset, -offset)  // bottom-right    
		);

		float kernel[9] = float[](
			1.0 / 16, 2.0 / 16, 1.0 / 16,
			2.0 / 16, 4.0 / 16, 2.0 / 16,
			1.0 / 16, 2.0 / 16, 1.0 / 16
		);
		
		vec3 sampleTex[9];
		for(int i = 0; i < 9; i++)
		{
			sampleTex[i] = vec3(texture(image, fs_in.texCoords + offsets[i]));
		}
		vec3 col = vec3(0.0);
		for(int i = 0; i < 9; i++)
			col += sampleTex[i] * kernel[i];
		
		FragColor = vec4(col, 1.0);
	}
}
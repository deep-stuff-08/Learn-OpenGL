#version 460 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in VS_OUT {
	vec3 N;
	vec3 L;
	vec3 V;
	vec2 Tex;
} gs_in[];

out GS_OUT {
	vec3 N;
	vec3 L;
	vec3 V;
	vec2 Tex;
} gs_out;

layout(location = 3)uniform float time;
layout(location = 4)uniform float magnitude;

vec4 explode(vec4 position, vec3 normal) {
	vec3 direction = normal * ((sin(time) + 1.0) / 2.0) * magnitude;
	return position + vec4(direction, 1.0);
}

void main(void) {
	vec3 normal = normalize(cross(gl_in[0].gl_Position.xyz - gl_in[1].gl_Position.xyz, gl_in[2].gl_Position.xyz - gl_in[1].gl_Position.xyz));

	gl_Position = explode(gl_in[0].gl_Position, normal);
	gs_out.N = gs_in[0].N;
	gs_out.L = gs_in[0].L;
	gs_out.V = gs_in[0].V;
	gs_out.Tex = gs_in[0].Tex;
	EmitVertex();
	gl_Position = explode(gl_in[1].gl_Position, normal);
	gs_out.N = gs_in[1].N;
	gs_out.L = gs_in[1].L;
	gs_out.V = gs_in[1].V;
	gs_out.Tex = gs_in[1].Tex;
	EmitVertex();
	gl_Position = explode(gl_in[2].gl_Position, normal);
	gs_out.N = gs_in[2].N;
	gs_out.L = gs_in[2].L;
	gs_out.V = gs_in[2].V;
	gs_out.Tex = gs_in[2].Tex;
	EmitVertex();
	EndPrimitive();
}
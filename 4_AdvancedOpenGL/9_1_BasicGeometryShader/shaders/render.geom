#version 460 core

layout(points) in;
layout(triangle_strip, max_vertices = 5)out;

in VS_OUT {
	vec4 color;
} gs_in[];

out GS_OUT {
	vec4 color;
} gs_out;

void main(void) {
	gs_out.color = gs_in[0].color;
	gl_Position = gl_in[0].gl_Position + vec4(-0.2, -0.2, 0.0, 0.0);
	EmitVertex();   
	gl_Position = gl_in[0].gl_Position + vec4( 0.2, -0.2, 0.0, 0.0);
	EmitVertex();
	gl_Position = gl_in[0].gl_Position + vec4(-0.2,  0.2, 0.0, 0.0);
	EmitVertex();
	gl_Position = gl_in[0].gl_Position + vec4( 0.2,  0.2, 0.0, 0.0);
	EmitVertex();
	gl_Position = gl_in[0].gl_Position + vec4( 0.0,  0.4, 0.0, 0.0);
	gs_out.color = vec4(1.0, 1.0, 1.0, 1.0);
	EmitVertex();
	EndPrimitive();
}
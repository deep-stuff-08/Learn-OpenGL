#version 460 core

layout(triangles) in;
layout(line_strip, max_vertices = 6) out;

layout(location = 0)uniform mat4 pMat;

in vec3 normals[];

void extrudeNormal(int index) {
	gl_Position = pMat * gl_in[index].gl_Position;
	EmitVertex();
	gl_Position = pMat * (gl_in[index].gl_Position + vec4(normalize(normals[index]), 0.0) * 0.2);
	EmitVertex();
	EndPrimitive();
}

void main(void) {
	extrudeNormal(0);
	extrudeNormal(1);
	extrudeNormal(2);
}
#version 330 core

layout(triangles) in;
layout (triangle_strip, max_vertices=3) out;

flat in int vInstance[3];
flat out int vCurrentLayer;

in vec2 uv[3];
out vec2 TexCoord;

void main()
{
	gl_Layer = vInstance[0];
	vCurrentLayer = vInstance[0];

	gl_Position = gl_in[0].gl_Position;
	TexCoord = uv[0];
	EmitVertex();

	gl_Position = gl_in[1].gl_Position;
	TexCoord = uv[1];
	EmitVertex();

	gl_Position = gl_in[2].gl_Position;
	TexCoord = uv[2];
	EmitVertex();

	EndPrimitive();
}
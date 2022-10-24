#version 330 core
#extension GL_ARB_viewport_array : enable
layout(triangles) in;
layout (triangle_strip, max_vertices=9) out;

uniform mat4 u_proj_view[3];
uniform uvec3 u_dimensions;

flat out uint depth;

void main()
{
	// Three axis voxelization
	for (int i = 0; i < 3; ++i)
	{
		gl_Layer = i;
		gl_ViewportIndex = i;
		depth = u_dimensions[i];

		for(int j = 0; j < gl_in.length(); j++)
		{
			// copy attributes
			gl_Position = u_proj_view[i] * gl_in[j].gl_Position;
	
			// done with the vertex
			EmitVertex();
		}
		// done with the primitive
		EndPrimitive();
	}
}

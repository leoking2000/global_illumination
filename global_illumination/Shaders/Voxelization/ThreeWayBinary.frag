#version 330 core
layout(location = 0) out uvec4 out_voxel;

flat in uint depth;

void main()
{
    // get z
	float z = clamp(gl_FragCoord.z, 0, 1); 

	// get an unsigned int in the range of [0, depth - 1];
	uint position = uint(floor(z*depth));

    out_voxel = uvec4(0u, 0u, 0u, 0u);
	out_voxel[position / 32u] = 1u << (position % 32u);
}



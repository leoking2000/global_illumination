#version 330 core
//#extension GL_EXT_gpu_shader4 : enable
layout(location = 0) out uvec4 out_voxel;

uniform usampler2DArray u_sampler_three_way;
uniform ivec3 u_dimensions;

#define AXIS_X 0
#define AXIS_Y 1
#define AXIS_Z 2

void main()
{
	// get the integer coordinates of the fragment (in window relative coordinates)
	ivec2 coords = ivec2(floor(gl_FragCoord.xy));

	// first obtain the voxels for the current fragment on the XY plane
	uvec4 tex_colorZ = texelFetch(u_sampler_three_way, ivec3(coords.xy, AXIS_Z), 0);
	uvec4 result = uvec4(0u,0u,0u,0u);
	
	// we work on the XY plane, so we parse on the Z axis
	// the loop is in the range [1, depth]  in order for the bit shifting operations to work
	// and to avoid unnecessary if's

	// we work on the XY plane, so we parse on the Z axis
	for(int z = 0; z < u_dimensions.z; ++z)
	{
		// encode current z onto a uvec4 (in similar fashion to the 1st pass shader)
		uint bitPositionZ = uint(z);
		uvec4 bitZ = uvec4(0u,0u,0u,0u);
		
		// get an unsigned vec4 containing the current position (marked as 1)
		bitZ[bitPositionZ / 32u] = 1u << (bitPositionZ % 32u);
				
		// use AND to mark whether the current position has been set as occupied
		uvec4 resultZ = tex_colorZ & bitZ;

		// check if the current position is marked as occupied
		bool result_axis = (resultZ.r | resultZ.g | resultZ.b | resultZ.a) > 0u;
		
		// encode current y onto a uvec4 (in similar fashion to the 1st pass shader)
		uvec4 tex_colorY = texelFetch(u_sampler_three_way, ivec3(z, u_dimensions.x - 1 - coords.x, AXIS_Y), 0);
		
		uint bitPositionY = uint(u_dimensions.y) - 1u - uint(coords.y);
		uvec4 bitY = uvec4(0u,0u,0u,0u);
		
		// get an unsigned vec4 containing the current position (marked as 1)
		bitY[bitPositionY / 32u] = 1u << (bitPositionY % 32u);
			
		// use AND to mark whether the current position has been set as occupied
		uvec4 resultY = tex_colorY & bitY;

		// check if the current position is marked as occupied
		result_axis = result_axis || (resultY.r | resultY.g | resultY.b | resultY.a) > 0u;

		// encode current x onto a uvec4 (in similar fashion to the 1st pass shader)
		uvec4 tex_colorX = texelFetch(u_sampler_three_way, ivec3(u_dimensions.z - 1 - z, coords.y, AXIS_X), 0);
		
		uint bitPositionX = uint(coords.x);
		uvec4 bitX = uvec4(0u,0u,0u,0u);
		
		// get an unsigned vec4 containing the current position (marked as 1)
		bitX[bitPositionX / 32u] = 1u << (bitPositionX % 32u);
					
		// use AND to mark whether the current position has been set as occupied
		uvec4 resultX = tex_colorX & bitX;

		// check if the current position is marked as occupied
		result_axis = result_axis || (resultX.r | resultX.g | resultX.b | resultX.a) > 0u;
		
		// mark current bit as occupied (bitZ is always 1)
		result |= (result_axis == true) ? bitZ : uvec4(0u,0u,0u,0u);

	}

	out_voxel = result;
}

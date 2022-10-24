#version 330 core
layout(location = 0) in vec3 apos;
layout(location = 1) in vec2 atex_cord;
layout(location = 2) in vec3 anormal;

uniform usampler2D u_voxels;

uniform mat4 u_proj_view_matrix;
uniform mat4 u_scale;

uniform ivec3 u_size;
uniform vec3 uniform_bbox_min;
uniform vec3 uniform_bbox_max;

out vec3 position;
out vec3 normal;
flat out int ok;

bool checkCRCValidityGeo(in ivec3 grid_position)
{
	// sample the voxels in the neighborhood of pos
	ivec2 pos = ivec2(grid_position.xy);

	uvec4 master_voxel = texelFetch(u_voxels, pos.xy, 0);
	
	// check if the CRC voxel is valid
	uvec4 slice = master_voxel;
	uint voxel_z = uint(u_size.z - grid_position.z - 1);

	// get an unsigned vec4 containing the current position (marked as 1)
	uvec4 slicePos = uvec4(0u,0u,0u,0u);
	slicePos[voxel_z / 32u] = 1u << (voxel_z % 32u);

	// use AND to mark whether the current position has been set as occupied
	uvec4 res = slice & slicePos;
	
	// check if the current position is marked as occupied
	return ((res.r | res.g | res.b | res.a) > 0u) ;
}

void main()
{
	ivec3 grid_position;
	grid_position.z = gl_InstanceID / ( u_size.x * u_size.y );
	grid_position.y = ( gl_InstanceID / u_size.x ) % u_size.y;
	grid_position.x = gl_InstanceID % u_size.x;

    ok = (!checkCRCValidityGeo(grid_position)) ? 1 : 0;

    vec3 stratum = (uniform_bbox_max - uniform_bbox_min) / vec3(u_size);

	vec3 pos_wcs = vec3(u_scale * vec4(apos, 1.0)).xyz;
	pos_wcs += uniform_bbox_min + (grid_position + 0.5) * stratum;

    gl_Position = u_proj_view_matrix * vec4(pos_wcs,1);

	normal = anormal;
}
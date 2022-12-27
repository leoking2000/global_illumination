#version 330
out vec4 out_pos;

uniform usampler2D u_voxels;

uniform ivec3 u_size;
uniform vec3 uniform_bbox_min;
uniform vec3 uniform_bbox_max;

in vec2 TexCoord;
flat in int vCurrentLayer;

vec3 GridSpaceToWSC(ivec3 voxel_pos)
{
    vec3 stratum = (uniform_bbox_max - uniform_bbox_min) / vec3(u_size);
    vec3 pos_wcs = uniform_bbox_min + (voxel_pos + 0.5) * stratum;
    return pos_wcs;
}

bool checkCRCValidityGeo(in ivec3 grid_position)
{
	// sample the voxels in the neighborhood of pos
	ivec2 pos = ivec2(grid_position.xy);

	int lod = 0;
	uvec4 master_voxel = texelFetch(u_voxels, pos.xy, lod);
	
	// check if the CRC voxel is valid
	uvec4 slice = master_voxel;
	uint voxel_z = uint(u_size.z - grid_position.z - 1);

	// get an unsigned vec4 containing the current position (marked as 1)
	uvec4 slicePos = uvec4(0u,0u,0u,0u);
	slicePos[voxel_z / 32u] = 1u << (voxel_z % 32u);

	// use AND to mark whether the current position has been set as occupied
	uvec4 res = slice & slicePos;
	
	// check if the current position is marked as occupied
	return ((res.r | res.g | res.b | res.a) > 0u);
}

int EncodeOffset(int x, int y, int z, int exist)
{
    return (x << 24) | (y << 16) | (z << 8) | exist;
}
//EncodeOffset(voxel_pos.x, voxel_pos.y, voxel_pos.z, int(checkCRCValidityGeo(voxel_pos)))

void main()
{
    ivec3 voxel_pos = ivec3(gl_FragCoord.xy, vCurrentLayer+0.5);
    vec3 pos_wcs = GridSpaceToWSC(voxel_pos);

	int exist = (!checkCRCValidityGeo(voxel_pos)) ? 1 : 0;

    out_pos = vec4(pos_wcs, exist);
}
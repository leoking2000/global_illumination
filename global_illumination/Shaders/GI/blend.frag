#version 330 core
layout(location = 0) out vec4 out_data0;
layout(location = 1) out vec4 out_data1;
layout(location = 2) out vec4 out_data2;
layout(location = 3) out vec4 out_data3;
layout(location = 4) out vec4 out_data4;
layout(location = 5) out vec4 out_data5;
layout(location = 6) out vec4 out_data6;

flat in int vCurrentLayer;

uniform sampler3D u_caching_data_current[7];
uniform sampler3D u_caching_data_prev[7];

uniform usampler2D u_voxels_musked;
uniform ivec3 u_size;

uniform vec3 u_bbox_max;
uniform vec3 u_bbox_min;
uniform vec3 u_stratum;
uniform float u_blend_factor;

bool checkCRCValidityGeo(in ivec3 grid_position)
{
    // sample the voxels in the neighborhood of pos
    ivec2 pos = ivec2(grid_position.xy);

    int lod = 0;
    uvec4 master_voxel = texelFetch(u_voxels_musked, pos.xy, lod);
    
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

void main()
{
    // get the coordinates of the current voxel
    vec3 voxel_coord = vec3(gl_FragCoord.xy, vCurrentLayer+0.5);

    // if voxel is inactive, discard
    if(!checkCRCValidityGeo(ivec3(voxel_coord))){
        discard;
    }

    vec3 extents = u_bbox_max - u_bbox_min;
    // position is initialized at the center of the current voxel
    vec3 pos_wcs = u_bbox_min + vec3(voxel_coord) * u_stratum;

    vec3 uvw = (pos_wcs - u_bbox_min) / extents;

    vec4 prev_data0        = texture(u_caching_data_prev[0], uvw);
    vec4 prev_data1        = texture(u_caching_data_prev[1], uvw);
    vec4 prev_data2        = texture(u_caching_data_prev[2], uvw);
    vec4 prev_data3        = texture(u_caching_data_prev[3], uvw);
    vec4 prev_data4        = texture(u_caching_data_prev[4], uvw);
    vec4 prev_data5        = texture(u_caching_data_prev[5], uvw);
    vec4 prev_data6        = texture(u_caching_data_prev[6], uvw);

    vec4 cur_data0        = texture(u_caching_data_current[0], uvw);
    vec4 cur_data1        = texture(u_caching_data_current[1], uvw);
    vec4 cur_data2        = texture(u_caching_data_current[2], uvw);
    vec4 cur_data3        = texture(u_caching_data_current[3], uvw);
    vec4 cur_data4        = texture(u_caching_data_current[4], uvw);
    vec4 cur_data5        = texture(u_caching_data_current[5], uvw);
    vec4 cur_data6        = texture(u_caching_data_current[6], uvw);

    // 0 deactivated
    // 1 only prev frame
    out_data0            = mix(cur_data0, prev_data0, u_blend_factor);
    out_data1            = mix(cur_data1, prev_data1, u_blend_factor);
    out_data2            = mix(cur_data2, prev_data2, u_blend_factor);
    out_data3            = mix(cur_data3, prev_data3, u_blend_factor);
    out_data4            = mix(cur_data4, prev_data4, u_blend_factor);
    out_data5            = mix(cur_data5, prev_data5, u_blend_factor);
    out_data6            = mix(cur_data6, prev_data6, u_blend_factor);
    out_data6.a          = 1.0;
}
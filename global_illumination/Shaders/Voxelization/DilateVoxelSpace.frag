#version 420 core

layout(location = 0) out uvec4 out_voxel;

layout (binding = 0) uniform atomic_uint voxelCounter;

uniform usampler2D u_voxels;
uniform ivec3 u_dimensions;

uint getBit(in uvec4 master,in uint i)
{
    if(i >= 128u)
    {
        return 0u;
    }

    return (master[i / 32u] >> i) & 1u;
}

void numberOfActiveBits(in uvec4 master)
{
    uint count;

    for(uint i = 1u; i < 127u; i++)
    {
        if(getBit(master, i) != 0u){
            count = atomicCounterIncrement(voxelCounter);
        }
    }
}

void main()
{
    // sample the voxels in the neighborhood of pos
    ivec2 pos = ivec2(floor(gl_FragCoord.xy));

    uvec4 voxel1 = texelFetch(u_voxels, pos.xy, 0);
    uvec4 voxel2 = texelFetch(u_voxels, pos.xy + ivec2(-1, 0), 0);
    uvec4 voxel3 = texelFetch(u_voxels, pos.xy + ivec2(-1, 1), 0);
    uvec4 voxel4 = texelFetch(u_voxels, pos.xy + ivec2(-1,-1), 0);
    uvec4 voxel5 = texelFetch(u_voxels, pos.xy + ivec2( 0, 1), 0);
    uvec4 voxel6 = texelFetch(u_voxels, pos.xy + ivec2( 0,-1), 0);
    uvec4 voxel7 = texelFetch(u_voxels, pos.xy + ivec2( 1, 1), 0);
    uvec4 voxel8 = texelFetch(u_voxels, pos.xy + ivec2( 1, 0), 0);
    uvec4 voxel9 = texelFetch(u_voxels, pos.xy + ivec2( 1,-1), 0);

    uvec4 master = voxel1 | voxel2 | voxel3 | voxel4 | voxel5 | voxel6 | voxel7 | voxel8 | voxel9;
    uvec4 out_v = master;

    if(getBit(master, 0u) == 0u && getBit(master, 1u) != 0u)
    {
        out_v[0u / 32u] |= 1u << 0u;
    }

    for(uint i = 1u; i < 127u; i++)
    {
        if(getBit(master, i) == 0u && (getBit(master, i - 1u) != 0u || getBit(master, i + 1u) != 0u))
        {
            out_v[i / 32u] |= 1u << i;
        }
    }

    if(getBit(master, 127u) == 0u && getBit(master, 126u) != 0u)
    {
        out_v[127u / 32u] |= 1u << 127u;
    }

    numberOfActiveBits(out_v);

    out_voxel = out_v;
}
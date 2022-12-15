#version 330
layout(location = 0) out vec4 out_gid;

uniform usampler2D u_voxels;
uniform ivec3 u_dimensions;

#define getBit(master, i) (master[i / 32u] >> i) & 1u;

void main()
{
    ivec2 pos = ivec2(floor(gl_FragCoord.xy));

    uvec4 slice = texelFetch(u_voxels, pos.xy, 0);

    for(int i = 0; i < 128; i++)
    {

    }


}
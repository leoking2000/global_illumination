#version 330 core
layout(location = 0) out vec4 out_data0;
layout(location = 1) out vec4 out_data1;
layout(location = 2) out vec4 out_data2;
layout(location = 3) out vec4 out_data3;
layout(location = 4) out vec4 out_data4;
layout(location = 5) out vec4 out_data5;
layout(location = 6) out vec4 out_data6;

flat in int vCurrentLayer;

// voxel
uniform usampler2D u_voxels_musked;
uniform ivec3 u_size;
uniform vec3 u_bbox_max;
uniform vec3 u_bbox_min;
uniform vec3 u_stratum;
uniform vec3 u_occlusion_bextents;

// RSM
uniform sampler2D u_RSM_pos;
uniform sampler2D u_RSM_flux;
uniform sampler2D u_RSM_normal;

// light
uniform vec3 u_light_pos;
uniform vec3 u_light_dir;
uniform mat4 u_light_projection_view;

// settings
uniform int u_num_samples;
uniform float u_spread;
#define NUM_OCCLUSION_SAMPLES 10

// random 
uniform vec2 u_samples_2d[200];
uniform vec3 u_samples_3d[200];

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

vec2 ShadowProjection( in vec3 point_WCS )
{
    vec4 pos_LCS = u_light_projection_view * vec4(point_WCS + vec3(0.01,0.01,0.01),1.0);
    pos_LCS /= pos_LCS.w;
    float reverse = sign(dot(u_light_dir,point_WCS - u_light_pos));
    vec2 uv = vec2(reverse * 0.5 * pos_LCS.xy + 0.5);
    return clamp(uv,vec2(0.0,0.0),vec2(1.0,1.0));
}

void projectDirToOrder2Basis ( const in vec3 dir, out float sh00,
               out float sh1_1, out float sh10, out float sh11,
               out float sh2_2, out float sh2_1, out float sh20, out float sh21, out float sh22)
{
    sh00    = 0.282094792;
    sh1_1   = -0.488602512 * dir.y;
    sh10    = 0.488602512 * dir.z;
    sh11    = -0.488602512 * dir.x;
    sh2_2   = 1.092548431 * dir.y*dir.x;
    sh2_1   = 1.092548431 * dir.y*dir.z;
    sh20    = 0.946174695 * dir.z * dir.z - 0.315391565;
    sh21    = 1.092548431 * dir.x*dir.z;
    sh22    = 0.546274215 * (dir.x*dir.x - dir.y*dir.y);
}

void encodeRadianceToSH (in vec3 dir, in vec3 L, out vec3 L00,
                    out vec3 L1_1, out vec3 L10, out vec3 L11,
                    out vec3 L2_2, out vec3 L2_1, out vec3 L20, out vec3 L21, out vec3 L22)
{
    float sh00, sh1_1, sh10, sh11, sh2_2, sh2_1, sh20, sh21, sh22;
    projectDirToOrder2Basis ( dir, sh00, sh1_1, sh10, sh11, sh2_2, sh2_1, sh20, sh21, sh22 );

    L00   = L * sh00; 
    L1_1  = L * sh1_1;
    L10   = L * sh10; 
    L11   = L * sh11; 
    L2_2  = L * sh2_2;
    L2_1  = L * sh2_1;
    L20   = L * sh20; 
    L21   = L * sh21; 
    L22   = L * sh22; 
}

void main()
{
    // get the coordinates of the current voxel
    vec3 voxel_coord = vec3(gl_FragCoord.xy, vCurrentLayer+0.5);

    // if voxel is inactive, discard
    if(!checkCRCValidityGeo(ivec3(voxel_coord))){
        discard;
    }

    // Spherical Harmonic variables
    vec3  SH_00   = vec3(0.0,0.0,0.0);
    vec3  SH_1_1  = vec3(0.0,0.0,0.0);
    vec3  SH_10   = vec3(0.0,0.0,0.0);
    vec3  SH_11   = vec3(0.0,0.0,0.0);
    vec3  SH_2_2  = vec3(0.0,0.0,0.0);
    vec3  SH_2_1  = vec3(0.0,0.0,0.0);
    vec3  SH_20   = vec3(0.0,0.0,0.0);
    vec3  SH_21   = vec3(0.0,0.0,0.0);
    vec3  SH_22   = vec3(0.0,0.0,0.0);

    // position is initialized at the center of the current voxel
    vec3 pos_wcs = u_bbox_min + vec3(voxel_coord) * u_stratum;
    vec2 l_uv = ShadowProjection(pos_wcs);

    float stratum_length = length(u_stratum);
    ivec2 rsm_size = textureSize(u_RSM_flux, 0);
    float inv_pdf = rsm_size.x * rsm_size.y * u_spread * u_spread;

    // clamp the sampling center based on the spread parameter
    vec2 uv_c = vec2(clamp(l_uv.xy, vec2(u_spread * 0.499), vec2(1.0 - u_spread * 0.499)));
    // the samples are generated in the range [0,1] with origin (0,0) so move the sampling center
    // to coincide with the sample origin
    uv_c -= (0.5 * vec2(u_spread));

    for (int i = 0; i < u_num_samples; ++i)
    {
        // get a random RSM sample in uv coordinates and project it in the RSM depth buffervs
        vec2 uv = uv_c + u_samples_2d[i] * u_spread;

        // get the position of the projected sample, its color and its normal in WCS
        vec3 s_pos = texture(u_RSM_pos, uv).xyz;
        vec3 s_flux = texture(u_RSM_flux, uv).rgb;
        vec3 s_normal = normalize(texture(u_RSM_normal, uv).xyz);

        // get a random position in wcs
        // pos_wcs is located at the center of the voxel and the samples are in the range of [0,1]
        vec3 p = pos_wcs + (u_samples_3d[i] * vec3(0.5)) * u_stratum;

        float dist = distance(p, s_pos);
        if (dist < stratum_length) continue;

        vec3 dir = (dist <= 0.007) ? vec3(0,0,0) : normalize (p - s_pos);

        // calculate the form factor and the radiance of the sample position
        float dotprod = max(dot(dir, s_normal), 0.0);
        if (dotprod < 0.07) continue;

        // OCCLUSION

        float vis = 1.0;
        // number of intermediate steps
        // create a jittered offset in the range of [0, 1]
        vec3 voxel_marching_dir = normalize(s_pos - p);
        vec3 occlusion_jitter = voxel_marching_dir * u_samples_2d[i].x * stratum_length * 0.5;
        vec3 offset = voxel_marching_dir * stratum_length;
        // fix for the case where the distance to the RSM is smaller than the offset. 0.4 is similar to 1.0 / 2.5 (total offsets)
        float length_offset = min(length(s_pos - p) * 0.4, length(offset));
        offset = normalize(offset) * length_offset;
        vec3 start_pos = p + offset + occlusion_jitter;
        vec3 end_pos = s_pos - offset;
        vec3 voxel_marching_step = voxel_marching_dir;
        voxel_marching_step *= length(end_pos - start_pos) / (NUM_OCCLUSION_SAMPLES - 1);
        vec2 texelStep = vec2(0.0) / textureSize(u_voxels_musked, 0);
        vec3 sample_pos;
        vec3 voxel_pos;
        int cur_i = -1;

        for (int j = 0; j < NUM_OCCLUSION_SAMPLES; j++)
        {
            sample_pos = start_pos + j * voxel_marching_step - 0.1 * voxel_marching_dir;
            voxel_pos = (sample_pos - u_bbox_min) / u_occlusion_bextents;

            uvec4 slice = textureLod(u_voxels_musked, voxel_pos.xy, 0);
            uint voxel_z = uint(u_size.z - floor((voxel_pos.z * u_size.z) + 0.0) - 1);

            // get an unsigned vec4 containing the current position (marked as 1)
            uvec4 slicePos = uvec4(0u);
            slicePos[voxel_z / 32u] = 1u << (voxel_z % 32u);

            // use AND to mark whether the current position has been set as occupied
            uvec4 res = slice & slicePos;

            // check if the current position is marked as occupied
            if ((res.r | res.g | res.b | res.a) > 0u) 
            {
                vis = 0.0;
                break;
            }
        }

        // end OCCLUSION

        float FF = dotprod / float(0.01 + dist * dist);
        vec3 color = vis * s_flux * FF / (3.14159);

        // project the radiance onto spherical harmonics basis functions
        // store the radiance in the incoming direction
        vec3 sh_00, sh_1_1, sh_10, sh_11, sh_2_2, sh_2_1, sh_20, sh_21, sh_22;

        encodeRadianceToSH(-dir, color, sh_00, sh_1_1, sh_10, sh_11, sh_2_2, sh_2_1, sh_20, sh_21, sh_22);

        SH_00  += sh_00;
        SH_1_1 += sh_1_1;
        SH_10  += sh_10;
        SH_11  += sh_11;
        SH_2_2 += sh_2_2;
        SH_2_1 += sh_2_1;
        SH_20  += sh_20;
        SH_21  += sh_21;
        SH_22  += sh_22;
    }

    float divsamples = 1.0 / float(u_num_samples);

    // scale by 1/100 as flux is premultiplied by 100 upon saving in the RSM to avoid truncation errors.
    float mult = inv_pdf * divsamples * 0.01; 

    out_data0       = vec4 (SH_00.r,    SH_00.g,    SH_00.b,    SH_1_1.r)   * mult;
    out_data1       = vec4 (SH_1_1.g,   SH_1_1.b,   SH_10.r,    SH_10.g)    * mult;
    out_data2       = vec4 (SH_10.b,    SH_11.r,    SH_11.g,    SH_11.b)    * mult;
    out_data3       = vec4 (SH_2_2.r,   SH_2_2.g,   SH_2_2.b,   SH_2_1.r)   * mult;
    out_data4       = vec4 (SH_2_1.g,   SH_2_1.b,   SH_20.r,    SH_20.g)    * mult;
    out_data5       = vec4 (SH_20.b,    SH_21.r,    SH_21.g,    SH_21.b)    * mult;
    out_data6.rgb   = vec3 (SH_22.r,    SH_22.g,    SH_22.b)                * mult;
    out_data6.a     = 1.0;
}
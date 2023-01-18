#version 330
layout(location = 0) out vec4 out_data0;
layout(location = 1) out vec4 out_data1;
layout(location = 2) out vec4 out_data2;
layout(location = 3) out vec4 out_data3;
layout(location = 4) out vec4 out_data4;
layout(location = 5) out vec4 out_data5;
layout(location = 6) out vec4 out_data6;

flat in int vCurrentLayer;
const float pi = 3.1415936;

#define NUM_OCCLUSION_SAMPLES 4
#define MAX_PARAMETRIC_DIST 0.5

// voxel
uniform usampler2D u_voxels_musked;
uniform ivec3 u_size;
uniform vec3 u_bbox_max;
uniform vec3 u_bbox_min;
uniform vec3 u_stratum;

uniform int u_num_samples;

uniform sampler3D caching_data[7];
uniform vec3 u_samples_3d[500];

//uniform float u_average_albedo;

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

float rand1n(vec2 seed)
{
	highp vec3 abc = vec3(12.9898, 78.233, 43758.5453);
	highp float dt = dot(seed.xy, vec2(abc.x, abc.y));
	highp float sn = mod(dt, 2 * pi);
	return max(0.01, fract(sin(sn) * abc.z));
}

vec2 getSamplingSeed(vec2 unique)
{
	return unique.xy * 17;
}

vec3 dotSH (in vec3 direction, in vec3 L00,
                 in vec3 L1_1, in vec3 L10, in vec3 L11,
                 in vec3 L2_2, in vec3 L2_1, in vec3 L20, in vec3 L21, in vec3 L22 )
{
    // cosine lobe in SH
    float Y00  = 0.886226925;
    float Y1_1 = -1.02332671 * direction.y;
    float Y10  = 1.02332671 * direction.z;
    float Y11  = -1.02332671 * direction.x;
    float Y2_2 = 0.858086 * direction.x * direction.y;
    float Y2_1 = -0.858086 * direction.y * direction.z;
    float Y20  = 0.743125 * direction.z * direction.z - 0.247708;
    float Y21  = -0.858086 * direction.x * direction.z;
    float Y22  = 0.429043 * (direction.x * direction.x - direction.y * direction.y);


    // dot product in SH, return reconstructed irradiance
    vec3 irradiance = vec3(0);
    irradiance = Y00*L00 + Y1_1*L1_1 + Y10*L10 + Y11*L11 + Y2_2*L2_2 + Y2_1*L2_1 + Y20*L20 + Y21*L21 + Y22*L22;

    return irradiance;
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

    vec3  SH_00    = vec3(0.0,0.0,0.0);
    vec3  SH_1_1   = vec3(0.0,0.0,0.0);
    vec3  SH_10    = vec3(0.0,0.0,0.0); 
    vec3  SH_11    = vec3(0.0,0.0,0.0);
    vec3  SH_2_2   = vec3(0.0,0.0,0.0);
    vec3  SH_2_1   = vec3(0.0,0.0,0.0);
    vec3  SH_20    = vec3(0.0,0.0,0.0);
    vec3  SH_21    = vec3(0.0,0.0,0.0); 
    vec3  SH_22    = vec3(0.0,0.0,0.0);

    vec3 extents = u_bbox_max - u_bbox_min;
    vec3 normalized_extents = extents / max (extents.x, max(extents.y,extents.z) );
    // position is initialized at the center of the current voxel
    vec3 pos_wcs = u_bbox_min + vec3(voxel_coord) * u_stratum;

    vec3 uvw = (pos_wcs - u_bbox_min) / extents;

    float stratum_length = length(u_stratum);
    float surface_radius = stratum_length * 0.5;

    float dist;
    float occ_vox = 0.0;
    vec3 L00, L1_1, L10, L11, L2_2, L2_1, L20, L21, L22;
    vec4 data0, data1, data2, data3, data4, data5, data6;

    for (int i = 0; i < u_num_samples; i++)
    {
        vec3 uvw_dir = normalize(u_samples_3d[i] / normalized_extents);

        bool hit = true;
        vec3 final_sample_pos = (uvw + uvw_dir) * MAX_PARAMETRIC_DIST;
        ivec3 isample_voxel = ivec3(final_sample_pos * u_size);

        vec3 sample_pos_wcs = u_bbox_min + vec3(isample_voxel + 0.5) * u_stratum;
        vec3 dir = sample_pos_wcs - pos_wcs;

        if (!hit || dot(dir, dir) < stratum_length * stratum_length){
            continue;
        }
        occ_vox += 1;

        dir = normalize(dir);

        vec3 L00, L1_1, L10, L11, L2_2, L2_1, L20, L21, L22;
        vec4 data0  = texture(caching_data[0], final_sample_pos);
        vec4 data1  = texture(caching_data[1], final_sample_pos);
        vec4 data2  = texture(caching_data[2], final_sample_pos);
        vec4 data3  = texture(caching_data[3], final_sample_pos);
        vec4 data4  = texture(caching_data[4], final_sample_pos);
        vec4 data5  = texture(caching_data[5], final_sample_pos);
        vec4 data6  = texture(caching_data[6], final_sample_pos);
        L00         = vec3(data0.x, data0.y, data0.z);
        L1_1        = vec3(data0.w, data1.x, data1.y);
        L10         = vec3(data1.z, data1.w, data2.x);
        L11         = vec3(data2.y, data2.z, data2.w);
        L2_2        = vec3(data3.x, data3.y, data3.z);
        L2_1        = vec3(data3.w, data4.x, data4.y);
        L20         = vec3(data4.z, data4.w, data5.x);
        L21         = vec3(data5.y, data5.z, data5.w);
        L22         = vec3(data6.x, data6.y, data6.z);

        // calculate the hemispherical integral using SH dot product
        vec3 sample_irradiance = dotSH (-dir, L00, L1_1, L10, L11, L2_2, L2_1, L20, L21, L22);
        encodeRadianceToSH(dir, sample_irradiance, L00, L1_1, L10, L11, L2_2, L2_1, L20, L21, L22);    

        SH_00  += L00;
        SH_1_1 += L1_1;
        SH_10  += L10;
        SH_11  += L11;
        SH_2_2 += L2_2;
        SH_2_1 += L2_1;
        SH_20  += L20;
        SH_21  += L21;
        SH_22  += L22;
    }

    float mult = 4.0 * 1.0 / float( 1 + occ_vox );

    out_data0       = vec4 (SH_00.r  ,SH_00.g  ,SH_00.b  ,SH_1_1.r )   * mult + texture(caching_data[0],uvw);
    out_data1       = vec4 (SH_1_1.g ,SH_1_1.b ,SH_10.r  ,SH_10.g  )   * mult + texture(caching_data[1],uvw);
    out_data2       = vec4 (SH_10.b  ,SH_11.r  ,SH_11.g  ,SH_11.b  )   * mult + texture(caching_data[2],uvw);
    out_data3       = vec4 (SH_2_2.r ,SH_2_2.g ,SH_2_2.b ,SH_2_1.r )   * mult + texture(caching_data[3],uvw);
    out_data4       = vec4 (SH_2_1.g ,SH_2_1.b ,SH_20.r  ,SH_20.g  )   * mult + texture(caching_data[4],uvw);
    out_data5       = vec4 (SH_20.b  ,SH_21.r  ,SH_21.g  ,SH_21.b  )   * mult + texture(caching_data[5],uvw);
    out_data6.rgb   = vec3 (SH_22.r  ,SH_22.g  ,SH_22.b)               * mult + texture(caching_data[6],uvw).rgb;
    out_data6.a     = 1.0;
}
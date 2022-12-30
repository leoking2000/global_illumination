#version 330
layout(location = 0) out vec4 out_data0;
layout(location = 1) out vec4 out_data1;
layout(location = 2) out vec4 out_data2;
layout(location = 3) out vec4 out_data3;
layout(location = 4) out vec4 out_data4;
layout(location = 5) out vec4 out_data5;
layout(location = 6) out vec4 out_data6;

#define NUMBER_OF_SAMPLES 200

flat in int vCurrentLayer;

// voxels
uniform sampler3D u_voxels_3D;
uniform usampler2D u_voxels_musked;

// light
uniform vec3 u_light_pos;
uniform vec3 u_light_dir;
uniform mat4 u_light_projection_view;

// RSM
uniform sampler2D u_RSM_flux;
uniform sampler2D u_RSM_pos;
uniform sampler2D u_RSM_normal;

// RSM sample settings
uniform float u_spread;
uniform int u_num_samples;

// random
uniform vec2 u_samples_2d[NUMBER_OF_SAMPLES];
uniform vec3 u_samples_3d[NUMBER_OF_SAMPLES];

vec3 RGB2YCoCg(vec3 rgbColor)
{
    return vec3(
     rgbColor.r * 0.25 + rgbColor.g * 0.5 + rgbColor.b * 0.25,
     rgbColor.r * 0.5                       - rgbColor.b * 0.5,
    -rgbColor.r * 0.25 + rgbColor.g * 0.5 - rgbColor.b * 0.25
    );
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

vec2 ShadowProjection(in vec3 pos_wcs)
{
    vec4 light_ndc = u_light_projection_view * vec4(pos_wcs + vec3(0.01, 0.01, 0.01), 1.0);

    // perspective division
    vec3 ndc = light_ndc.xyz / light_ndc.w; // normalize device cordince

    float reverse = sign(dot(u_light_dir, pos_wcs - u_light_pos));
    vec2 uv = vec2(reverse * 0.5 * ndc.xy + 0.5);

    return clamp(uv, vec2(0.0, 0.0), vec2(1.0, 1.0));
}

void main()
{
    // get the voxel info for the current grid position
    ivec3 grid_position = ivec3(gl_FragCoord.xy, vCurrentLayer+0.5);
    vec4 voxel = texelFetch(u_voxels_3D, grid_position, 0);

    // if the current voxel is inactive discard 
    if(voxel.w <= -1){
        discard;
    }

    // Spherical Harmonic variables
    vec3  SH_00     = vec3(0.0,0.0,0.0);
    vec3  SH_1_1    = vec3(0.0,0.0,0.0);
    vec3  SH_10     = vec3(0.0,0.0,0.0);
    vec3  SH_11     = vec3(0.0,0.0,0.0);
    vec3  SH_2_2    = vec3(0.0,0.0,0.0);
    vec3  SH_2_1    = vec3(0.0,0.0,0.0);
    vec3  SH_20     = vec3(0.0,0.0,0.0);
    vec3  SH_21     = vec3(0.0,0.0,0.0);
    vec3  SH_22     = vec3(0.0,0.0,0.0);

    vec3 pos_wcs = voxel.xyz; // pos_wcs is located at the center of the voxel
    // find the uv in the RSM buffer that corresponds to the voxel location
    vec2 rsm_uv = ShadowProjection(pos_wcs);

    // calculate the inv_pdf
    ivec2 rsm_size = textureSize(u_RSM_flux, 0);
    float inv_pdf = rsm_size.x * rsm_size.y * u_spread * u_spread;

    for (int i = 0; i < u_num_samples; ++i)
    {
        // get a random RSM sample in uv coordinates
        vec2 uv = rsm_uv + u_samples_2d[i] * u_spread;

        //float depth = texture(u_shadowMap, uv).r;
        vec3 s_pos = texture(u_RSM_pos, uv).xyz;
        vec3 s_flux = texture(u_RSM_flux, uv).rgb;
        vec3 s_normal = texture(u_RSM_normal, uv).xyz;

        // get a random position in wcs
        // pos_wcs is located at the center of the voxel and the samples are in the range of [0,1]
        vec3 p = pos_wcs + (u_samples_3d[i] * vec3(0.5));

        float dist = distance(p, s_pos);
        vec3 dir = (dist <= 0.007) ? vec3(0,0,0) : normalize (p - s_pos);

        // calculate the form factor and the radiance of the sample position
        float dotprod = max(dot(dir, s_normal), 0.0);
        if (dotprod < 0.07) continue;

        float vis = 1.0;

        // occusion ray ray-marching

        //TODO
        //TODO
        //TODO
        //TODO
        //TODO
        //TODO
        //TODO
        //TODO
        //TODO
        //TODO
        //TODO
        //TODO
        //TODO

        // end occusion ray-marching

        float FF = dotprod / float(0.01 + dist * dist);
        vec3 color = vis * s_flux * FF / (3.14159);

        // project the radiance onto spherical harmonics basis functions
        // store the radiance in the incoming direction
        vec3 sh_00, sh_1_1, sh_10, sh_11, sh_2_2, sh_2_1, sh_20, sh_21, sh_22;

        //color = RGB2YCoCg(color);
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

        out_data0            = vec4 (SH_00.r,    SH_00.g,    SH_00.b,    SH_1_1.r)   * mult;
        out_data1            = vec4 (SH_1_1.g,   SH_1_1.b,   SH_10.r,    SH_10.g)    * mult;
        out_data2            = vec4 (SH_10.b,    SH_11.r,    SH_11.g,    SH_11.b)    * mult;
        out_data3            = vec4 (SH_2_2.r,   SH_2_2.g,   SH_2_2.b,   SH_2_1.r)   * mult;
        out_data4            = vec4 (SH_2_1.g,   SH_2_1.b,   SH_20.r,    SH_20.g)    * mult;
        out_data5            = vec4 (SH_20.b,    SH_21.r,    SH_21.g,    SH_21.b)    * mult;
        out_data6.rgb        = vec3 (SH_22.r,    SH_22.g,    SH_22.b)                * mult;
        out_data6.a          = 1.0;

}
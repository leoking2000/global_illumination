#version 330 core
out vec4 out_color;
in vec2 tex_cord;

const float PI = 3.14159265359;

uniform sampler3D u_caching_data[7];

uniform sampler2D u_tex_albedo;
uniform sampler2D u_tex_pos;
uniform sampler2D u_tex_normal;
uniform sampler2D u_tex_mask;
uniform sampler2D u_tex_depth;

uniform mat4 u_proj_view_inv;

uniform vec3 u_bbox_min;
uniform vec3 u_bbox_max;
uniform float u_bbox_min_side;
uniform vec3 u_stratum;

// settings
uniform float u_factor;

vec3 PointCSS2WCS(in vec3 p_ccs)
{
    vec4 p_wcs = u_proj_view_inv * vec4(p_ccs, 1.0);
    return p_wcs.xyz/p_wcs.w;
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
    float Y2_1 = 0.858086 * direction.y * direction.z;
    float Y20  = 0.743125 * direction.z * direction.z - 0.247708;
    float Y21  = 0.858086 * direction.x * direction.z;
    float Y22  = 0.429043 * (direction.x * direction.x - direction.y * direction.y);

    // dot product in SH, return reconstructed irradiance
    vec3 irradiance = vec3(0);
    irradiance = Y00*L00 + Y1_1*L1_1 + Y10*L10 + Y11*L11 + Y2_2*L2_2 + Y2_1*L2_1 + Y20*L20 + Y21*L21 + Y22*L22;
    return irradiance;
}

vec3 YCoCg2RGB(vec3 YCoCg)
{
    return vec3(
    YCoCg.r + YCoCg.g - YCoCg.b,
    YCoCg.r              + YCoCg.b,
    YCoCg.r - YCoCg.g - YCoCg.b
    );
}

void main()
{
    float depth = texture(u_tex_depth, tex_cord).r;
    if(depth == 1.0){
        discard;
    }

    vec3 extents = u_bbox_max - u_bbox_min;

    vec3 sz = textureSize(u_caching_data[0],0);
    vec3 pos_wcs = texture(u_tex_pos, tex_cord).xyz;

    vec3 uvw = (pos_wcs - u_bbox_min) / extents;
    vec3 normal_wcs = texture(u_tex_normal, tex_cord).xyz;

    vec3 normalized_extents = extents / max (extents.x, max(extents.y,extents.z) );

    vec3 v_rand = vec3(0.5, 0.5, 0.5);
    vec3 v_1 = normalize(cross(normal_wcs, v_rand));
    vec3 v_2 = cross(normal_wcs, v_1);

    vec3 D[4];
    D[0] = vec3(0.1,0.0,0.0);

    for (int i = 1; i < 4; i++)
    {
        D[i] = vec3(0.1, 0.3*cos((i) * 6.2832/3.0), 0.3*sin((i) * 6.2832/3.0));
        D[i] = normalize(D[i] / normalized_extents);
    }

    vec3 gi_diffuse_color = vec3(0.0,0.0,0.0);
    for (int i = 0; i < 4; i++)
    {
        vec3 sdir = normal_wcs * D[i].x + v_1 * D[i].y + v_2 * D[i].z;
        vec3 uvw_new = (0.1 * normal_wcs + sdir) / sz + uvw;

        vec3 sample_irradiance = vec3(0);

        vec4 data0  = texture(u_caching_data[0], uvw_new);
        vec4 data1  = texture(u_caching_data[1], uvw_new);
        vec4 data2  = texture(u_caching_data[2], uvw_new);
        vec4 data3  = texture(u_caching_data[3], uvw_new);
        vec4 data4  = texture(u_caching_data[4], uvw_new);
        vec4 data5  = texture(u_caching_data[5], uvw_new);
        vec4 data6  = texture(u_caching_data[6], uvw_new);
        vec3 L00    = vec3(data0.x, data0.y, data0.z);
        vec3 L1_1   = vec3(data0.w, data1.x, data1.y);
        vec3 L10    = vec3(data1.z, data1.w, data2.x);
        vec3 L11    = vec3(data2.y, data2.z, data2.w);
        vec3 L2_2   = vec3(data3.x, data3.y, data3.z);
        vec3 L2_1   = vec3(data3.w, data4.x, data4.y);
        vec3 L20    = vec3(data4.z, data4.w, data5.x);
        vec3 L21    = vec3(data5.y, data5.z, data5.w);
        vec3 L22    = vec3(data6.x, data6.y, data6.z);
        // calculate the hemispherical integral using SH dot product
        sample_irradiance = 
            dotSH(normal_wcs, L00, L1_1, L10, L11, L2_2, L2_1, L20, L21, L22);

        //sample_irradiance = YCoCg2RGB(sample_irradiance);

        gi_diffuse_color += sample_irradiance;
    }
    gi_diffuse_color *= 0.25;

    vec3 kd = texture(u_tex_albedo, tex_cord).rgb;
    float metallic = texture(u_tex_mask, tex_cord).y;
    gi_diffuse_color *= (1 - metallic) * kd / PI;

    // for a moving volume, the final GI color is a blend
    // between the reconstructed irradiance and a constant
    // ambient color
    // blend starts at 4 voxels before the box's minimum side and
    // reaches maximum at 2 voxels before the box's minimum side

    // center of the moving volume in wcs
    vec3 box_center = (u_bbox_max + u_bbox_min) * 0.5;
    vec3 min_side_blend_start = box_center + u_bbox_min_side - u_stratum * 2.0;
    vec3 min_side_blend_stop = box_center +  u_bbox_min_side - u_stratum * 0.0;

    // distance of current point from center
    float dist = distance(box_center, pos_wcs);
    // distance of current point from start of blending
    float dist_min = distance(box_center, min_side_blend_start);
    // distance of current point to stop of blending
    float dist_max = distance(box_center, min_side_blend_stop);

    // normalize it
    float dist_norm = (dist - dist_min) / (dist_max - dist_min);
    dist_norm = clamp(dist_norm, 0.0, 1.0);

    // blend
    vec3 ambient = vec3(0.1) * kd / 3.14159;
    gi_diffuse_color.rgb = mix(gi_diffuse_color.rgb, ambient, dist_norm);

    gi_diffuse_color = max(gi_diffuse_color, vec3(0));

    out_color = vec4(gi_diffuse_color * u_factor, 1.0);
}
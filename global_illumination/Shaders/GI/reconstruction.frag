#version 330 core
out vec4 out_color;
in vec2 uv;

const float PI = 3.14159265359;

// G-buffer
uniform sampler2D u_tex_albedo;
uniform sampler2D u_tex_pos;
uniform sampler2D u_tex_normal;
uniform sampler2D u_tex_mask;
uniform sampler2D u_tex_depth;

// camera
uniform vec3 u_camera_pos;
uniform vec3 u_camera_dir;

// light
struct Light
{
    int type; // 0 -> directional, 1 -> spotlight
    vec3 pos;
    vec3 dir;

    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    vec3 radiance;
};

uniform Light u_light;

// shadows
uniform mat4 u_light_projection_view;
uniform sampler2D u_shadowMap;
uniform float u_shadow_bias;

// caching
uniform sampler3D caching_data[7];

uniform ivec3 u_size;
uniform vec3 u_bbox_min;
uniform vec3 u_bbox_max;

/**********************************
    CACHING
***********************************/

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

/**********************************
    SHADOW
***********************************/

float shadow_pcf2x2_weighted(vec3 light_space_xyz)
{
    ivec2 shadow_map_size = textureSize(u_shadowMap, 0);
    float xOffset = 1.0 / shadow_map_size.x;
    float yOffset = 1.0 / shadow_map_size.y;

    // compute the weights of the neighboring pixels
    vec2 uv = light_space_xyz.xy - vec2(xOffset, yOffset);
    float u_ratio = mod(uv.x, xOffset) / xOffset;
    float v_ratio = mod(uv.y, yOffset) / yOffset;
    float u_opposite = 1 - u_ratio;
    float v_opposite = 1 - v_ratio;

    // compute the distance with a small bias
    float z = light_space_xyz.z - u_shadow_bias;

    // compute the shadow percentage
    float bottomLeft  = (texture(u_shadowMap, uv).r > z)                         ? u_opposite : 0.0;
    float bottomRight = (texture(u_shadowMap, uv + vec2(xOffset, 0)).r > z)      ? u_ratio : 0.0;
    float topLeft     = (texture(u_shadowMap, uv + vec2(0, yOffset), 0).r > z)   ? u_opposite : 0.0;
    float topRight    =  texture(u_shadowMap, uv + vec2(xOffset, yOffset)).r > z ? u_ratio : 0.0;

    float factor      = (bottomLeft + bottomRight) * v_opposite + (topLeft + topRight) * v_ratio;
    return factor;
}

float shadow(vec3 pos)
{
    vec4 light_ndc = u_light_projection_view * vec4(pos, 1.0);

    // perspective division
    vec3 ndc = light_ndc.xyz / light_ndc.w; // normalize device cordince
    // transform to [0,1] range
    ndc = ndc * 0.5 + 0.5;

    float outside_light = u_light.type == 0 ? 1.0 : 0.0;

    // check that we are inside light clipping frustum
    if (ndc.x < 0.0) return outside_light;
    if (ndc.y < 0.0) return outside_light;
    if (ndc.x > 1.0) return outside_light;
    if (ndc.y > 1.0) return outside_light;
    if (ndc.z < 0.0) return outside_light;
    if (ndc.z > 1.0) return outside_light;

    // sample shadow map
    return shadow_pcf2x2_weighted(ndc);
}

/**********************************
    LIGHT
***********************************/

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
    
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    
    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
    
    return ggx1 * ggx2;
}

vec3 CalculateSurfToLight(vec3 pos)
{
    if(u_light.type == 0)
    {
        return -u_light.dir;
    }

    return normalize(u_light.pos - pos);
}

float CalculateAttenuation(vec3 pos)
{
    float distance    = length(u_light.pos - pos);
    float attenuation = 1.0 / (u_light.constant + u_light.linear * distance + u_light.quadratic * (distance * distance));

    return attenuation;
}

vec3 DoLightingCalculations(vec3 albedo, vec3 pos, vec3 normal, vec3 mask,float intensity, vec3 gi_diffuse_color)
{
    float roughness = mask.r;
    float metallic = mask.g;

    float shadow_value = shadow(pos);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    vec3 surfToEye = normalize(u_camera_pos - pos.xyz);
    vec3 surfToLight = CalculateSurfToLight(pos);
    vec3 halfwayDir = normalize(surfToLight + surfToEye);

    float attenuation = u_light.type == 0 ? 1.0 : CalculateAttenuation(pos);
    vec3 radiance  =  u_light.radiance * attenuation;

    // cook-torrance brdf
    float NDF = DistributionGGX(normal, halfwayDir, roughness);        
    float G   = GeometrySmith(normal, surfToEye, surfToLight, roughness);      
    vec3 F    = fresnelSchlick(max(dot(halfwayDir, surfToEye), 0.0), F0);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(normal, surfToEye), 0.0) * max(dot(normal, surfToLight), 0.0) + 0.0001;
    vec3 specular     = numerator / denominator;

    float NdotL = max(dot(normal, surfToLight), 0.0);
    vec3 Lo = (kD * albedo / PI + specular) * radiance * NdotL;

    gi_diffuse_color *= Lo;
    gi_diffuse_color *= 0.25;
    gi_diffuse_color = max(gi_diffuse_color, vec3(0));

    vec3 color = (Lo * shadow_value * intensity) + gi_diffuse_color;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2)); 

    return color;
}

void main(void)
{
    float depth = texture(u_tex_depth, uv).r;
    if(depth == 1.0) discard;

    vec3 gi_diffuse_color = vec3(0.0,0.0,0.0);

    vec3 pos_wcs = texture(u_tex_pos, uv).xyz;
    vec3 normal_wcs = normalize(texture(u_tex_normal, uv).xyz);

    vec3 extents = u_bbox_max - u_bbox_min;
    vec3 sz = textureSize(caching_data[0], 0);

    vec3 uvw = (pos_wcs - u_bbox_min) / extents;

    vec3 normalized_extents = extents / max (extents.x, max(extents.y,extents.z) );
    
    vec3 v_rand = vec3(0.5, 0.5, 0.5);
    vec3 v_1 = normalize(cross(normal_wcs ,v_rand));
    vec3 v_2 = cross(normal_wcs ,v_1);
            
    vec3 D[4];
    D[0] = vec3(0.1,0.0,0.0);

    for (int i = 1; i < 4; i++)
    {
        D[i] = vec3(0.1, 0.3*cos((i) * 6.2832/3.0), 0.3*sin((i) * 6.2832/3.0));
        D[i] = normalize(D[i] / normalized_extents);
    }


    for (int i = 0; i < 4; i++)
    {
        vec3 sdir = normal_wcs * D[i].x + v_1 * D[i].y + v_2 * D[i].z;
        vec3 uvw_new = (0.1*normal_wcs + sdir)/sz + uvw;

        vec3 sample_irradiance = vec3(0);

        vec4 data0        = texture(caching_data[0], uvw_new);    
        vec4 data1        = texture(caching_data[1], uvw_new);
        vec4 data2        = texture(caching_data[2], uvw_new);
        vec4 data3        = texture(caching_data[3], uvw_new);
        vec4 data4        = texture(caching_data[4], uvw_new);
        vec4 data5        = texture(caching_data[5], uvw_new);
        vec4 data6        = texture(caching_data[6], uvw_new);
        vec3 L00        = vec3(data0.x, data0.y, data0.z);
        vec3 L1_1        = vec3(data0.w, data1.x, data1.y);
        vec3 L10        = vec3(data1.z, data1.w, data2.x);
        vec3 L11        = vec3(data2.y, data2.z, data2.w);
        vec3 L2_2        = vec3(data3.x, data3.y, data3.z);
        vec3 L2_1        = vec3(data3.w, data4.x, data4.y);
        vec3 L20        = vec3(data4.z, data4.w, data5.x);
        vec3 L21        = vec3(data5.y, data5.z, data5.w);
        vec3 L22        = vec3(data6.x, data6.y, data6.z);
        // calculate the hemispherical integral using SH dot product
        // saturation is set to 1.0 here
        sample_irradiance = 1.0 * dotSH (normal_wcs, L00, L1_1, L10, L11, L2_2, L2_1, L20, L21, L22);

        gi_diffuse_color += sample_irradiance;
    }

    vec3 albedo = texture(u_tex_albedo, uv).rgb;
    vec3 mask = texture(u_tex_mask, uv).rgb;

    vec3 color;
    if(u_light.type == 0)
    {
        color = DoLightingCalculations(albedo, pos_wcs, normal_wcs, mask, 1.0, gi_diffuse_color);
    }
    else
    {
        float theta = dot(CalculateSurfToLight(pos_wcs), normalize(-u_light.dir));
        float epsilon   = u_light.cutOff - u_light.outerCutOff;
        float intensity = clamp((theta - u_light.outerCutOff) / epsilon, 0.0, 1.0);

        color = DoLightingCalculations(albedo, pos_wcs, normal_wcs, mask, intensity, gi_diffuse_color);
    }

    out_color = vec4(color, 1);
}
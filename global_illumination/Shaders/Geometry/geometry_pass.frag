#version 330
layout(location = 0) out vec4 out_albedo;
layout(location = 1) out vec4 out_pos;
layout(location = 2) out vec4 out_normal;
layout(location = 3) out vec4 out_mask;

uniform vec3 u_albedo;
uniform sampler2D u_albedoMap;
uniform int u_HasAlbedoMap;

uniform int u_HasNormalMap;
uniform sampler2D u_normalMap;

uniform float u_roughness;
uniform float u_metallic;

in vec3 position;
in vec2 tex_cord;
in mat3 TBN;

vec4 GetAlbedo()
{
    if(u_HasAlbedoMap > 0)
    {
        return texture(u_albedoMap, tex_cord);
    }

    return vec4(u_albedo, 1);
}

vec3 CalculateNormal()
{
    if(u_HasNormalMap > 0)
    {
        vec3 nmap = texture(u_normalMap, tex_cord).rgb;
        nmap = nmap * 2.0 - 1.0;
        vec3 normal = normalize(TBN * nmap);

        return normal;
    }

    return normalize(TBN[2]);
}

void main()
{
    vec4 albedo = GetAlbedo();

    if(albedo.a == 0.0) discard;

    out_albedo = albedo;
    out_pos = vec4(position, 1);
    out_normal = vec4(CalculateNormal(), 1);
    out_mask = vec4(u_roughness, u_metallic, 0.0, 0.0);
}
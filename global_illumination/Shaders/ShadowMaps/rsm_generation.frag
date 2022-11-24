#version 330
layout(location = 0) out vec4 out_flux;
layout(location = 1) out vec4 out_pos;
layout(location = 2) out vec4 out_normal;

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

vec3 CalculateSurfToLight(vec3 pos)
{
	if(u_light.type == 0)
	{
		return -u_light.dir;
	}

	return normalize(u_light.pos - pos);
}

vec3 CalculateFlux(vec3 albedo, vec3 pos)
{
    if(u_light.type == 0){
        return u_light.radiance * albedo;
    }

    float theta = dot(CalculateSurfToLight(pos), normalize(-u_light.dir));
	float epsilon   = u_light.cutOff - u_light.outerCutOff;
	float intensity = clamp((theta - u_light.outerCutOff) / epsilon, 0.0, 1.0);

    return u_light.radiance * albedo * intensity;
}

void main()
{
    vec4 albedo = GetAlbedo();

    if(albedo.a == 0.0) discard;

    out_flux = vec4(CalculateFlux(albedo.rgb, position), 1);
    out_pos = vec4(position, 1);
    out_normal = vec4(CalculateNormal(), 1);
}
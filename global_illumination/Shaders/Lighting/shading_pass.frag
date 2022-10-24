#version 330

const float PI = 3.14159265359;

uniform sampler2D u_tex_albedo;
uniform sampler2D u_tex_pos;
uniform sampler2D u_tex_normal;
uniform sampler2D u_tex_mask;
uniform sampler2D u_tex_depth;

// camera
uniform vec3 u_camera_pos;
uniform vec3 u_camera_dir;

// light
uniform vec3 u_light_dir;

// shadows
uniform mat4 u_light_projection_view;
uniform sampler2D u_shadowMap;
uniform float u_shadow_bias;

in vec2 tex_cord;

out vec4 out_color;

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

	// check that we are inside light clipping frustum
	if (ndc.x < 0.0) return 1.0;
	if (ndc.y < 0.0) return 1.0;
	if (ndc.x > 1.0) return 1.0;
	if (ndc.y > 1.0) return 1.0;
    if (ndc.z < 0.0) return 1.0;
    if (ndc.z > 1.0) return 1.0;

	// sample shadow map
	return shadow_pcf2x2_weighted(ndc);
}

/**********************************
	LIGHT
***********************************/

vec3 CalculateSurfToLight()
{
	return -u_light_dir;
}

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

/**********************************
	MAIN
***********************************/

void main()
{
	vec3 lightColor = vec3(1);

	float d = texture(u_tex_depth, tex_cord).r;
	if(d == 1.0) discard;

	vec3 albedo = texture(u_tex_albedo, tex_cord).xyz;
	vec3 pos = texture(u_tex_pos, tex_cord).xyz;
	vec3 normal = texture(u_tex_normal, tex_cord).xyz;

	vec3 mask = texture(u_tex_mask, tex_cord).rgb;
	float roughness = mask.r;
	float metallic = mask.g;
	float ao = 1.0;

	float shadow_value = shadow(pos);

	vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

	vec3 surfToEye = normalize(u_camera_pos - pos.xyz);
	vec3 surfToLight = CalculateSurfToLight();
	vec3 halfwayDir = normalize(surfToLight + surfToEye);

	float attenuation = 1.0;
	vec3 radiance  = lightColor * attenuation;

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

	vec3 ambient = vec3(0.01) * albedo * ao;
    vec3 color = ambient + Lo * shadow_value;

	color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2)); 

    out_color = vec4(color, 1.0);
}
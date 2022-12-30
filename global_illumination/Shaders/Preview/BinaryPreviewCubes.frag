#version 330 core
layout(location = 0) out vec4 out_albedo;
layout(location = 1) out vec4 out_pos;
layout(location = 2) out vec4 out_normal;
layout(location = 3) out vec4 out_mask;

in vec3 position;
in vec3 normal;
flat in int ok;

uniform int u_show_all;

void main(void)
{
	if(ok >= 0) 
    {
        out_albedo = vec4(1.0, 0.0, 0.0, 1.0); // red
        out_pos = vec4(position, 1.0);
        out_normal = vec4(normal, 1.0);
        float roughness = 0.5;
        float metallic = 0;
        out_mask = vec4(roughness, metallic, 0.0, 0.0);
    }
	else
	{
        if(u_show_all == 0){
            discard;
        }

        out_albedo = vec4(0.0, 0.0, 1.0, 1.0); // blue
        out_pos = vec4(position, 1.0);
        out_normal = vec4(normal, 1.0);
        float roughness = 0.5;
        float metallic = 0;
        out_mask = vec4(roughness, metallic, 0.0, 0.0);
    }
}
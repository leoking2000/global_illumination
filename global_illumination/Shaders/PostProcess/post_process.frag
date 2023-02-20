#version 330 core

uniform sampler2D uniform_texture;
uniform int u_depth;

in vec2 uv;

out vec4 out_color;

float near_plane = 0.1;
float far_plane = 1000;
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

void main(void)
{
    vec3 color = texture(uniform_texture, uv).rgb;

    if(u_depth == 0)
    {
        color = pow(color, vec3(1.0/2.2));
        out_color = vec4(color, 1.0);
    }
    else
    {
        //out_color = vec4(vec3(LinearizeDepth(color.r) / far_plane), 1.0);
        out_color = vec4(color.r, color.r, color.r, 1.0);
    }
  
}
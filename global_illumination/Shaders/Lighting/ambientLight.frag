#version 330
layout(location = 0) out vec4 out_color;

in vec2 tex_cord;

uniform sampler2D u_tex_albedo;
uniform sampler2D u_tex_depth;

uniform vec3 u_ambient_color;
uniform vec3 u_background_color;

void main()
{
    float depth = texture2D(u_tex_depth, tex_cord).r;
    vec3 albedo = texture2D(u_tex_albedo, tex_cord).rgb;

    vec3 ambColor = (depth == 1.0) ? u_background_color : albedo * u_ambient_color;
    out_color = vec4(ambColor, 1);
}

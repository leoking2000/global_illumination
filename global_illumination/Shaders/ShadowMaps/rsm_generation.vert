#version 330
layout(location = 0) in vec3 apos;
layout(location = 1) in vec2 atex_cord;
layout(location = 2) in vec3 anormal;
layout(location = 3) in vec3 atangent;
layout(location = 4) in vec3 abitangent;

uniform mat4 u_model_matrix;
uniform mat4 u_proj_view_model_matrix;
uniform mat4 u_normal_matrix;

out vec3 position;
out vec2 tex_cord;
out mat3 TBN;

void main()
{
    position  = (u_model_matrix * vec4(apos, 1.0) ).xyz;
    tex_cord = atex_cord;

    vec3 T = normalize(mat3(u_normal_matrix) * atangent);
    vec3 B = normalize(mat3(u_normal_matrix) * abitangent);
    vec3 N = normalize(mat3(u_normal_matrix) * anormal);
    TBN = mat3(T, B, N);

    gl_Position = u_proj_view_model_matrix * vec4(apos, 1.0);
}
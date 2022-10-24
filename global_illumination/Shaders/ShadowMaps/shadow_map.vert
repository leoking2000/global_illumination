#version 330
layout(location = 0) in vec3 apos;

uniform mat4 u_projViewModel;

void main()
{
    gl_Position = u_projViewModel * vec4(apos, 1.0);
}
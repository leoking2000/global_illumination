#version 330 core
layout(location = 0) in vec2 coord2d;

flat out int vInstance;

void main(void)
{
    gl_Position = vec4(coord2d, 0.0, 1.0);

    vInstance = gl_InstanceID;
}
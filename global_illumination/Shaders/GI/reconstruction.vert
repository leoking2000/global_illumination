#version 330 core
layout(location = 0) in vec2 apos;

out vec2 tex_cord;

void main(void)
{
    gl_Position = vec4(apos, 0.0, 1.0);

    tex_cord = (apos + 1.0) / 2.0;
}
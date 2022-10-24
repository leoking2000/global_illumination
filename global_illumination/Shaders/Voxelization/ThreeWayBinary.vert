#version 330 core
#extension GL_EXT_gpu_shader4 : enable
layout(location = 0) in vec3 apos;

uniform mat4 u_model_matrix;

void main(void)
{
	gl_Position = u_model_matrix * vec4(apos, 1.0);
}

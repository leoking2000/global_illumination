#pragma once
#include "Global/Types.h"

namespace GL
{
	class Light
	{
	public:
		glm::mat4 m_light_proj = glm::ortho(-15.0f, 15.0f, -15.0f, 15.0f, 0.1f, 100.0f);
		glm::vec3 m_light_pos = glm::vec3(0.0f, 12.0f, 24.0f);
		glm::vec3 m_light_dir = glm::normalize(glm::vec3(0.0f, -0.369f, -0.929f));
		f32 m_shadow_bias = 0.0006f;
	};
}
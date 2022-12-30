#pragma once
#include "Node.h"
#include "DrawStrategy.h"
#include <memory>

namespace GL
{
	enum LightType
	{
		DIRECTIONAL = 0,
		SPOTLIGHT = 1
	};

	class Light
	{
	public:
		Light();
		Light(const glm::vec3 pos, const glm::vec3 dir, LightType type);
	public:
		void ImGui();

		glm::mat4 LightProj();
		glm::mat4 LightView();

		void SetUniforms(ShaderProgram& shader);
	public:
		glm::vec3 m_pos;
		glm::vec3 m_dir;
	private:
		glm::vec3 m_radiance = glm::vec3(1.0f);
		LightType m_type;

		f32 m_cutOffAngle = glm::radians(12.5f);
		f32 m_outercutOffAngle = glm::radians(17.5f);
	private:
		glm::mat4 m_light_proj;
		f32 m_shadow_bias = 0.0006f;
	};
}
#include "Light.h"
#include "imgui/imgui.h"

namespace GL
{
	Light::Light()
		:
		Light(glm::vec3(0.0f, 12.0f, 24.0f), glm::normalize(glm::vec3(0.0f, -0.369f, -0.929f)), LightType::DIRECTIONAL)
	{
	}

	Light::Light(const glm::vec3 pos, const glm::vec3 dir, LightType type)
		:
		m_pos(pos),
		m_dir(dir),
		m_type(type)
	{
		if (m_type == LightType::DIRECTIONAL)
		{
			m_light_proj = glm::ortho(-15.0f, 15.0f, -15.0f, 15.0f, 0.1f, 100.0f);
			m_radiance = glm::vec3(1.0f);
		}
		else
		{
			m_light_proj = glm::perspective<f32>(PI / 2, 1, 0.1f, 1000.0f);
			m_radiance = glm::vec3(9.0f);
		}
	}

	void Light::ImGui()
	{
		ImGui::Begin("Light");

		ImGui::DragFloat3("Pos", glm::value_ptr(m_pos));
		ImGui::DragFloat3("Dir", glm::value_ptr(m_dir), 0.1f, -1.0f, 1.0f);
		m_dir = glm::normalize(m_dir);
		ImGui::InputFloat("bias", &m_shadow_bias, 0.0001f, 0.0005f, "%f");
		ImGui::DragFloat3("radiance", glm::value_ptr(m_radiance));

		ImGui::End();
	}

	glm::mat4 Light::LightProj()
	{
		return m_light_proj;
	}

	glm::mat4 Light::LightView()
	{
		return glm::lookAt(m_pos, m_pos + m_dir, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	void Light::SetUniforms(ShaderProgram& shader)
	{
		shader.SetUniform("u_light.type", m_type == DIRECTIONAL ? 0 : 1);

		shader.SetUniform("u_light.pos", m_pos);
		shader.SetUniform("u_light.dir", m_dir);

		shader.SetUniform("u_light.cutOff", glm::cos(m_cutOffAngle));
		shader.SetUniform("u_light.outerCutOff", glm::cos(m_outercutOffAngle));

		shader.SetUniform("u_shadow_bias", m_shadow_bias);

		shader.SetUniform("u_light.constant", 1.0f);
		shader.SetUniform("u_light.linear", 0.35f);
		shader.SetUniform("u_light.quadratic", 0.44f);

		shader.SetUniform("u_light.radiance", m_radiance);
	}
}



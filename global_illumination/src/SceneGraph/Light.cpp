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
		m_dir(glm::normalize(dir)),
		m_type(type)
	{
		if (m_type == LightType::DIRECTIONAL)
		{
			m_light_proj = glm::ortho(-12.0f, 12.0f, -12.0f, 12.0f, 0.1f, 100.0f);
			m_radiance = glm::vec3(100.0f);
		}
		else
		{
			m_light_proj = glm::perspective<f32>(m_outercutOffAngle + PI / 8.0f, 1, 0.1f, 1000.0f);
			m_radiance = glm::vec3(100.0f);
		}
	}

	void Light::ImGui()
	{
		if (ImGui::CollapsingHeader("Light", true))
		{
			ImGui::DragFloat3("Pos", glm::value_ptr(m_pos));
			ImGui::DragFloat3("Dir", glm::value_ptr(m_dir), 0.1f, -1.0f, 1.0f);
			m_dir = glm::normalize(m_dir);
			ImGui::InputFloat("bias", &m_shadow_bias, 0.0001f, 0.0005f, "%f");
			ImGui::DragFloat3("radiance", glm::value_ptr(m_radiance));
			ImGui::DragFloat("Cut Off Angle (in radians)", &m_cutOffAngle, 0.01f, 0.0f, PI / 2);

			f32 old_angle = m_outercutOffAngle;
			ImGui::DragFloat("Outer Cut Off Angle (in radians)", &m_outercutOffAngle, 0.01f, 0.0f, PI / 2);

			if (m_type == LightType::DIRECTIONAL)
			{
				m_light_proj = glm::ortho(-12.0f, 12.0f, -12.0f, 12.0f, 0.1f, 100.0f);
			}
			else if (old_angle != m_outercutOffAngle)
			{
				m_light_proj = glm::perspective<f32>(m_outercutOffAngle + PI / 8.0f, 1, 0.1f, 1000.0f);
			}
		}
	}

	glm::mat4 Light::LightProj()
	{
		return m_light_proj;
	}

	glm::mat4 Light::LightView()
	{
		return glm::lookAt(m_pos, m_pos + m_dir, up);
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



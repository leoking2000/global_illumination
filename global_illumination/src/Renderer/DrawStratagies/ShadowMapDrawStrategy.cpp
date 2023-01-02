#include "Graphics/OpenGL.h"
#include "ShadowMapDrawStrategy.h"
#include "AssetManagement/AssetManagement.h"
#include "imgui/imgui.h"

namespace GL
{
	ShadowMapDrawStrategy::ShadowMapDrawStrategy(u32 shadowMapSize)
		:
		m_shadow_map(shadowMapSize, shadowMapSize, 0,TextureMinFiltering::MIN_NEAREST, TextureMagFiltering::MAG_NEAREST)
	{
		static bool shader_created = false;
		if (!shader_created) {
			m_shadow_shader = AssetManagement::CreateShader("ShadowMaps/shadow_map");
			shader_created = true;
		}
	}

	void ShadowMapDrawStrategy::ClearFrameBuffer()
	{
		m_shadow_map.Bind();

		glCall(glClear(GL_DEPTH_BUFFER_BIT));

		m_shadow_map.UnBind();
	}

	void ShadowMapDrawStrategy::Draw(const Model& model, const glm::mat4& proj, const glm::mat4& view, const glm::mat4& model_matrix) const
	{
		m_shadow_map.Bind();

		glCall(glViewport(0, 0, m_shadow_map.Width(), m_shadow_map.Height()));
		glCall(glEnable(GL_DEPTH_TEST));
		glCall(glDepthFunc(GL_LEQUAL));
		glCall(glClearDepth(1.0f));

		glCall(glEnable(GL_CULL_FACE));
		glCall(glCullFace(GL_BACK));
		glCall(glFrontFace(GL_CCW));

		ShaderProgram* shadow_shader = AssetManagement::GetShader(m_shadow_shader);

		shadow_shader->SetUniform("u_projViewModel",
			proj * view * model_matrix);

		shadow_shader->Bind();

		for (u32 i = 0; i < model.m_meshs.size(); i++)
		{
			AssetManagement::GetMesh(model.m_meshs[i])->Draw();
		}

		shadow_shader->UnBind();
		
		m_shadow_map.UnBind();
	}

	FrameBuffer& ShadowMapDrawStrategy::GetFrameBuffer()
	{
		return m_shadow_map;
	}
}


